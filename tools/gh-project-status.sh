#!/usr/bin/env bash
# Query a GitHub Projects (v2) board via the GraphQL API and print items grouped by status.
#
# Requires: gh (authenticated, with the `read:project` scope), curl, python3.
#   gh auth refresh -h github.com -s read:project
#
# Usage:
#   tools/gh-project-status.sh [org] [project-number]
#
# Note: uses curl rather than `gh api graphql` directly because `gh`'s Go HTTP
# client can fail TLS verification behind some sandboxed/proxied network
# setups; curl uses the system trust store and works there.

set -euo pipefail

ORG="${1:-ChilliHugger}"
PROJECT_NUMBER="${2:-4}"

TOKEN="$(gh config get -h github.com oauth_token)"
TMPDIR="${TMPDIR:-/tmp}"
TMP_QUERY="$(mktemp "$TMPDIR/gh-project-query.XXXXXX")"
TMP_RESPONSE="$(mktemp "$TMPDIR/gh-project-response.XXXXXX")"
trap 'rm -f "$TMP_QUERY" "$TMP_RESPONSE"' EXIT

cat > "$TMP_QUERY" <<EOF
{"query":"query(\$org: String!, \$number: Int!) { organization(login: \$org) { projectV2(number: \$number) { title items(first: 100) { nodes { fieldValues(first: 20) { nodes { ... on ProjectV2ItemFieldTextValue { text field { ... on ProjectV2FieldCommon { name } } } ... on ProjectV2ItemFieldSingleSelectValue { name field { ... on ProjectV2FieldCommon { name } } } ... on ProjectV2ItemFieldDateValue { date field { ... on ProjectV2FieldCommon { name } } } } } content { __typename ... on Issue { number title state labels(first: 10) { nodes { name } } assignees(first: 5) { nodes { login } } milestone { title } } ... on PullRequest { number title state labels(first: 10) { nodes { name } } assignees(first: 5) { nodes { login } } milestone { title } } ... on DraftIssue { title } } } } } } }","variables":{"org":"$ORG","number":$PROJECT_NUMBER}}
EOF

curl -sS \
  -H "Authorization: bearer $TOKEN" \
  -H "Content-Type: application/json" \
  https://api.github.com/graphql \
  -d @"$TMP_QUERY" \
  -o "$TMP_RESPONSE"

python3 - "$TMP_RESPONSE" <<'PYEOF'
import json
import sys

with open(sys.argv[1]) as f:
    data = json.load(f)

if "errors" in data:
    print(json.dumps(data["errors"], indent=2), file=sys.stderr)
    sys.exit(1)

project = data["data"]["organization"]["projectV2"]
items = project["items"]["nodes"]

by_status = {}
for it in items:
    content = it.get("content") or {}
    fields = {
        fv["field"]["name"]: fv.get("text") or fv.get("name") or fv.get("date")
        for fv in it["fieldValues"]["nodes"]
        if fv.get("field")
    }
    status = fields.get("Status", "(no status)")
    title = content.get("title", "(untitled)")
    number = content.get("number")
    kind = content.get("__typename", "DraftIssue")
    labels = [l["name"] for l in content.get("labels", {}).get("nodes", [])] if content.get("labels") else []
    assignees = [a["login"] for a in content.get("assignees", {}).get("nodes", [])] if content.get("assignees") else []
    milestone = content.get("milestone", {}).get("title") if content.get("milestone") else None
    by_status.setdefault(status, []).append((number, kind, title, labels, assignees, milestone))

print(f"{project['title']} — {len(items)} items\n")

for status, rows in by_status.items():
    print(f"== {status} ({len(rows)}) ==")
    for number, kind, title, labels, assignees, milestone in rows:
        tag = f"#{number}" if number else "(draft)"
        suffix = " [PR]" if kind == "PullRequest" else ""
        line = f"  {tag}{suffix} {title}"
        if labels:
            line += f" | labels={labels}"
        if assignees:
            line += f" | assignees={assignees}"
        if milestone:
            line += f" | milestone={milestone}"
        print(line)
    print()
PYEOF
