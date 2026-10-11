//
//  panel_quest.cpp
//  citadel
//

#include "../axmol_sdk.h"
#include "panel_quest.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../system/keyboardmanager.h"
#include "../ui/uihelper.h"
#include "../system/tmemanager.h"
#include "../ui/characters/uisinglelord.h"

#include <algorithm>
#include <utility>
#include <vector>

#if defined(_CITADEL_)

USING_NS_AX;
USING_NS_AX_UI;
USING_NS_TME;

const panel_quest::quest_choice_t panel_quest::questChoices[] = {
    { QS_RECRUIT,   "quest_recruit",    SS_QUEST_NAME_RECRUIT,  SS_QUEST_ASK_RECRUIT,   false },
    { QS_JOIN,      "quest_join",       SS_QUEST_NAME_JOIN,     SS_QUEST_ASK_JOIN,      false },
    { QS_KILL,      "quest_kill",       SS_QUEST_NAME_KILL,     SS_QUEST_ASK_KILL,      false },
    { QS_RESCUE,    "quest_rescue",     SS_QUEST_NAME_RESCUE,   SS_QUEST_ASK_RESCUE,    false },
    { QS_FOLLOW,    "quest_follow",     SS_QUEST_NAME_FOLLOW,   SS_QUEST_ASK_FOLLOW,    false },
    { QS_GOTO,      "quest_goto",       SS_QUEST_NAME_GOTO,     SS_QUEST_PICK_GOTO,     true  },
    { QS_GUARD,     "quest_guard",      SS_QUEST_NAME_GUARD,    SS_QUEST_PICK_GUARD,    true  },
    { QS_SEIZE,     "quest_seize",      SS_QUEST_NAME_SEIZE,    SS_QUEST_ASK_SEIZE,     false },
    { QS_FIND,      "quest_find",       SS_QUEST_NAME_FIND,     SS_QUEST_ASK_FIND,      false },
    { QS_TAKE,      "quest_take",       SS_QUEST_NAME_TAKE,     SS_QUEST_ASK_TAKE,      false },
    { QS_DESTROY,   "quest_destroy",    SS_QUEST_NAME_DESTROY,  SS_QUEST_ASK_DESTROY,   false },
    { QS_REST,      "quest_rest",       SS_QUEST_NAME_REST,     SS_QUEST_REST,          false },
};

const panel_quest::quest_choice_t* panel_quest::choiceFor( mxquest_t quest )
{
    for ( const auto& choice : questChoices ) {
        if ( choice.quest == quest )
            return &choice;
    }
    return nullptr;
}

bool panel_quest::init()
{
    if ( !uipanel::init() )
        return false;

    setBackground(_clrWhite);

    auto size = safeArea->getContentSize();

    title = Label::createWithTTF(uihelper::font_config_big, "");
    title->getFontAtlas()->setAntiAliasTexParameters();
    title->setTextColor(Color4B(_clrDarkRed));
    uihelper::AddTopLeft(safeArea, title, RES(32), RES(32));

    status = Label::createWithTTF(uihelper::font_config_big, "");
    status->getFontAtlas()->setAntiAliasTexParameters();
    status->setTextColor(Color4B(_clrBlue));
    status->setAlignment(TextHAlignment::LEFT);
    status->setMaxLineWidth(size.width - RES(64));
    status->enableWrap(true);
    uihelper::AddTopLeft(safeArea, status, RES(32), RES(96));

    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(safeArea, look, RES(10), RES(10));

    back = uihelper::CreateBoxButton(Size(RES(200), RES(64)));
    back->setTitleText(TME_GetSystemString(TME_CurrentCharacter(), SS_QUEST_BACK));
    back->setTag(ID_QUEST);
    back->addClickEventListener(clickCallback);
    uihelper::AddBottomRight(safeArea, back, RES(32), RES(24));

    // the list sits just above the buttons
    contentBottom = std::max(look->getBoundingBox().getMaxY(), back->getBoundingBox().getMaxY()) + RES(8);

    uishortcutkeys::registerCallback(safeArea, clickCallback);
    addShortcutKey(ID_LOOK, K_LOOK);
    addShortcutKey(ID_LOOK, K_ESC);

    switch ( mr->questmodel.view ) {
        case questview::news:       showNews(); break;
        case questview::targets:    showTargets(); break;
        default:                    showQuests(); break;
    }
    return true;
}

void panel_quest::setHeading( const std::string& heading, const std::string& text )
{
    title->setString(heading);
    status->setString(text);
}

ax::ui::ScrollView* panel_quest::resetContent( f32 rowHeight, size_t rows )
{
    if ( content != nullptr )
        content->removeFromParent();
    if ( fadeTop != nullptr )
        fadeTop->removeFromParent();
    if ( fadeBottom != nullptr )
        fadeBottom->removeFromParent();
    content = fadeTop = fadeBottom = nullptr;

    auto size = safeArea->getContentSize();
    auto top = size.height - RES(96) - status->getContentSize().height - RES(8);
    auto bottom = contentBottom;
    auto area = Size(size.width - RES(64), std::max<f32>(top - bottom, RES(100)));
    auto height = rowHeight * rows;
    const auto fade = std::min<f32>(RES(48), area.height / 4);
    const bool scrolls = height > area.height;
    // extra space at the end so the last row can scroll clear of the fade
    if ( scrolls )
        height += fade;

    auto list = ScrollView::create();
    list->setDirection(ScrollView::Direction::VERTICAL);
    list->setContentSize(area);
    list->setInnerContainerSize(Size(area.width, std::max<f32>(area.height, height)));
    list->setScrollBarEnabled(scrolls);
    list->setBounceEnabled(scrolls);
    list->setPosition(Vec2(RES(32), bottom));
    safeArea->addChild(list);
    content = list;

    // fade the list out at the top and bottom when it scrolls
    if ( scrolls ) {
        auto colour = _clrWhite;
        fadeTop = uihelper::createVerticalGradient(colour, fade, fade, area.width, -1);
        fadeTop->setAnchorPoint(Vec2::ZERO);
        fadeTop->setPosition(Vec2(RES(32), bottom + area.height - fade));
        safeArea->addChild(fadeTop, list->getLocalZOrder() + 1);

        fadeBottom = uihelper::createVerticalGradient(colour, fade, fade, area.width, 1);
        fadeBottom->setAnchorPoint(Vec2::ZERO);
        fadeBottom->setPosition(Vec2(RES(32), bottom));
        safeArea->addChild(fadeBottom, list->getLocalZOrder() + 1);
    }
    return list;
}

ax::ui::Widget* panel_quest::addRow( ScrollView* list, size_t index, f32 rowHeight, mxid portrait, const std::string& text, layoutid_t tag )
{
    auto inner = list->getInnerContainerSize();
    auto y = inner.height - rowHeight * (index + 1);
    f32 x = 0;

    if ( portrait != IDT_NONE ) {
        auto face = uisinglelord::createWithLord(portrait);
        face->setTag(tag);
        face->addClickEventListener(clickCallback);
        face->setStatusImageVisible(false);
        face->setAnchorPoint(Vec2::ZERO);
        face->setPosition(Vec2(0, y));
        list->addChild(face);
        x = face->getBoundingBox().size.width + RES(16);
    }

    auto row = uihelper::CreateBoxButton(Size(inner.width - x, rowHeight - RES(16)));
    row->setTitleText(text);
    row->setTag(tag);
    row->addClickEventListener(clickCallback);
    row->setAnchorPoint(Vec2::ZERO);
    row->setPosition(Vec2(x, y + RES(8)));
    list->addChild(row);
    return row;
}

Layout* panel_quest::createGrid( ScrollView* area, size_t count, std::vector<Vec2>& centres )
{
    // same grid as the command window, without its background
    const int columns = 5;
    const int rows = ( (int)count + columns - 1 ) / columns;
    const f32 scale = mr->resolution->phoneScale();
    const auto cell = Size(RES(128) * scale, RES(128) * scale);
    const auto padding = Size(RES(32) * scale, RES(24) * scale);
    const auto boxSize = Size(cell.width * columns + padding.width * 2, cell.height * rows + padding.height * 2);
    auto size = area->getContentSize();

    auto box = Layout::create();
    box->setContentSize(boxSize);
    box->setAnchorPoint(uihelper::AnchorCenter);
    box->setPosition(Vec2(size.width / 2, size.height / 2));
    area->addChild(box);

    centres.clear();
    for ( size_t i = 0; i < count; i++ ) {
        centres.emplace_back(padding.width + cell.width * ( (i % columns) + 0.5f ),
                             boxSize.height - padding.height - cell.height * ( (i / columns) + 0.5f ));
    }
    return box;
}

void panel_quest::showQuests()
{
    mr->questmodel.view = questview::quests;
    auto& c = TME_CurrentCharacter();

    questinfo_t info {};
    Character_QuestInfo(c, info);
    setHeading(TME_GetSystemString(c, SS_QUEST_CHOOSE), TME_GetSystemString(c, SS_QUEST_STATUS));
    back->setVisible(false);

    auto area = resetContent(0, 0);
    std::vector<Vec2> centres;
    auto box = createGrid(area, NUMELE(questChoices), centres);

    for ( size_t i = 0; i < NUMELE(questChoices); i++ ) {
        const auto& choice = questChoices[i];
        c_mxid targets;
        bool open = info.able && ( choice.place || choice.quest == QS_REST
            || Character_QuestTargets(c, choice.quest, targets) > 0 );
        bool current = info.quest == choice.quest || ( choice.quest == QS_REST && info.quest == QS_NONE );

        auto centre = centres[i];

        auto icon = uihelper::CreateImageButton(choice.icon, ID_QUEST_CHOICE + choice.quest, clickCallback);
        uihelper::setEnabled(icon, open);
        icon->setAnchorPoint(uihelper::AnchorCenter);
        icon->setPosition(centre);
        box->addChild(icon);

        auto label = uihelper::addButtonText(icon, TME_GetSystemString(c, choice.label));
        label->setScale(1.25f);
        if ( current )
            label->setTextColor(Color4B(_clrYellow));
    }
}

void panel_quest::showTargets()
{
    auto& c = TME_CurrentCharacter();
    auto choice = choiceFor(mr->questmodel.quest);
    if ( choice == nullptr ) {
        showQuests();
        return;
    }

    mr->questmodel.view = questview::targets;
    choices.Clear();
    Character_QuestTargets(c, choice->quest, choices);
    setHeading(c.longname, TME_GetSystemString(c, choice->prompt));
    back->setVisible(true);

    if ( choices.Count() > 0 && ID_TYPE(choices[0]) == IDT_OBJECT ) {
        showObjects();
        return;
    }

    if ( choices.Count() > 0 && ID_TYPE(choices[0]) == IDT_CHARACTER ) {
        std::vector<std::pair<std::string, mxid>> named;
        for ( auto id : choices ) {
            character other;
            TME_GetCharacter(other, id);
            named.emplace_back(other.longname, id);
        }
        std::sort(named.begin(), named.end());
        choices.Clear();
        for ( const auto& entry : named )
            choices.Add(entry.second);

        const f32 scale = mr->resolution->phoneScale();
        const f32 cellW = SELECT_ELEMENT_WIDTH * scale;
        const f32 cellH = RES(80) * scale;
        auto columns = std::max<size_t>(1, (size_t)( ( safeArea->getContentSize().width - RES(64) ) / cellW ));
        auto rows = ( choices.Count() + columns - 1 ) / columns;
        auto gallery = resetContent(cellH, rows);
        auto inner = gallery->getInnerContainerSize();
        for ( size_t i = 0; i < choices.Count(); i++ ) {
            auto face = uisinglelord::createWithLord(choices[i]);
            face->setTag((layoutid_t)(ID_QUEST_TARGET + i));
            face->addClickEventListener(clickCallback);
            face->setStatusImageVisible(false);
            face->setAnchorPoint(uihelper::AnchorCenter);
            face->setPosition(Vec2(cellW * ( (i % columns) + 0.5f ), inner.height - cellH * ( (i / columns) + 0.5f )));
            gallery->addChild(face);
        }
        return;
    }

    const f32 rowHeight = RES(150);
    auto list = resetContent(rowHeight, choices.Count());
    for ( size_t i = 0; i < choices.Count(); i++ ) {
        auto id = choices[i];
        mxid portrait = IDT_NONE;
        std::string text;
        switch ( ID_TYPE(id) ) {
            case IDT_CHARACTER: {
                character other;
                TME_GetCharacter(other, id);
                portrait = id;
                text = other.longname;
                break;
            }
            case IDT_STRONGHOLD: {
                stronghold keep;
                TME_GetStronghold(keep, id);
                text = StringUtils::format(TME_GetSystemString(c, SS_QUEST_STRONGHOLD).c_str(),
                                           TME_GetLocationText(keep.location).c_str(), (int)keep.totaltroops);
                break;
            }
            case IDT_OBJECT: {
                object thing;
                TME_GetObject(thing, id);
                text = thing.name;
                break;
            }
            default:
                break;
        }
        addRow(list, i, rowHeight, portrait, text, (layoutid_t)(ID_QUEST_TARGET + i));
    }
}

void panel_quest::showObjects()
{
    std::vector<Vec2> centres;
    auto box = createGrid(resetContent(0, 0), choices.Count(), centres);

    for ( size_t i = 0; i < choices.Count(); i++ ) {
        object thing;
        TME_GetObject(thing, choices[i]);
        auto data = static_cast<obj_data_t*>(thing.userdata);

        if ( data == nullptr || data->i_quest.empty() )
            UIDEBUG( "Quest: object '%s' has no quest icon in the .tme", TME_GetSymbol(choices[i]).c_str() );

        auto icon = uihelper::CreateImageButton(data != nullptr && !data->i_quest.empty() ? data->i_quest : "quest_find",
                                                (layoutid_t)(ID_QUEST_TARGET + i), clickCallback);
        icon->setAnchorPoint(uihelper::AnchorCenter);
        icon->setPosition(centres[i]);
        box->addChild(icon);

        // name label styled like the lord select titles
        auto label = Label::createWithTTF(uihelper::font_config_small, TME_GetObjectText(choices[i]));
        label->setTextColor(Color4B(_clrWhite));
        label->enableOutline(Color4B(_clrBlack), RES(1));
        label->getFontAtlas()->setAntiAliasTexParameters();
        label->setAnchorPoint(uihelper::AnchorCenter);
        label->setWidth(SELECT_ELEMENT_WIDTH - CONTENT_SCALE(4));
        label->setHeight(RES(FONT_SIZE_SMALL) * 3);
        label->setHorizontalAlignment(TextHAlignment::CENTER);
        label->setVerticalAlignment(TextVAlignment::BOTTOM);
        uihelper::AddBottomCenter(icon->getRendererNormal(), label);
    }
}

void panel_quest::showNews()
{
    mr->questmodel.view = questview::news;
    choices.Clear();
    TME_QuestNews(choices);
    if ( choices.Count() == 0 ) {
        showQuests();
        return;
    }

    setHeading(TME_GetSystemString(TME_CurrentCharacter(), SS_QUEST_NEWS_TITLE), TME_GetSystemString(TME_CurrentCharacter(), SS_QUEST_NEWS_HEADING));
    back->setVisible(false);

    const f32 rowHeight = RES(150);
    auto list = resetContent(rowHeight, choices.Count());
    for ( size_t i = 0; i < choices.Count(); i++ ) {
        character lord;
        TME_GetCharacter(lord, choices[i]);
        auto row = static_cast<Button*>(addRow(list, i, rowHeight, lord.id,
                                               TME_GetSystemString(lord, SS_QUEST_NEWS_LINE),
                                               (layoutid_t)(ID_QUEST_TARGET + i)));
        row->setTitleFontSize(PHONE_SCALE(RES(FONT_SIZE_MEDIUM)));
    }
}

void panel_quest::choose( mxquest_t quest )
{
    auto choice = choiceFor(quest);
    if ( choice == nullptr )
        return;

    mr->questmodel.quest = quest;

    if ( quest == QS_REST ) {
        Character_SetQuest(TME_CurrentCharacter(), QS_REST, IDT_NONE);
        TME_RefreshCurrentCharacter();
        showQuests();
        return;
    }

    if ( choice->place ) {
        // the map chooses where; panel_map_detailed sets the quest and comes back here
        mr->questmodel.picking = true;
        mr->showPage(MODE_MAP_DETAILED);
        return;
    }

    showTargets();
}

void panel_quest::target( size_t index )
{
    if ( index >= choices.Count() )
        return;

    if ( mr->questmodel.view == questview::news ) {
        mr->questmodel.view = questview::quests;
        TME_SelectChar(choices[index]);
        TME_RefreshCurrentCharacter();
        showQuests();
        return;
    }

    Character_SetQuest(TME_CurrentCharacter(), mr->questmodel.quest, choices[index]);
    TME_RefreshCurrentCharacter();
    showQuests();
}

void panel_quest::OnNotification( Ref* sender )
{
    auto button = dynamic_cast<Widget*>(sender);
    if ( button == nullptr )
        return;

    auto id = (u32)button->getTag();

    if ( id == ID_LOOK ) {
        mr->questmodel.view = questview::quests;
        mr->look();
        return;
    }

    if ( id == ID_QUEST ) {
        showQuests();
        return;
    }

    if ( id >= ID_QUEST_TARGET ) {
        target(id - ID_QUEST_TARGET);
        return;
    }

    if ( id >= ID_QUEST_CHOICE ) {
        choose((mxquest_t)(id - ID_QUEST_CHOICE));
        return;
    }

    uipanel::OnNotification(sender);
}

#endif
