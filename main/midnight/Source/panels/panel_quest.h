//
//  panel_quest.h
//  citadel
//

#pragma once

#include "../ui/uipanel.h"
#include "../frontend/layout_id.h"

#include <vector>

#if defined(_CITADEL_)

class panel_quest : public uipanel
{
    using Label = ax::Label;
    using ScrollView = ax::ui::ScrollView;
    using Widget = ax::ui::Widget;
    using Layout = ax::ui::Layout;
    using Vec2 = ax::Vec2;

public:
    virtual bool init() override;

    CREATE_FUNC(panel_quest);

protected:
    struct quest_choice_t {
        mxquest_t   quest;
        LPCSTR      icon;       // its sprite - the 1995 game's own symbol
        u32         label;
        u32         prompt;     // the heading of its list of targets
        bool        place;      // chosen on the map rather than from a list
    };

    static const quest_choice_t questChoices[];
    static const quest_choice_t* choiceFor( mxquest_t quest );

    virtual void OnNotification( Ref* sender ) override;

    void showQuests();
    void showTargets();
    void showNews();
    void showObjects();

    // framed grid, laid out like the command window; cell centres are returned in box space
    Layout* createGrid( ScrollView* area, size_t count, std::vector<Vec2>& centres );
    void setHeading( const std::string& title, const std::string& text );
    ScrollView* resetContent( f32 rowHeight, size_t rows );
    Widget* addRow( ScrollView* list, size_t index, f32 rowHeight, mxid portrait, const std::string& text, layoutid_t tag );
    void choose( mxquest_t quest );
    void target( size_t index );

private:
    Label*      title = nullptr;
    Label*      status = nullptr;
    Node*       content = nullptr;
    Node*       fadeTop = nullptr;
    f32         contentBottom = 0;
    Node*       fadeBottom = nullptr;
    Button*     back = nullptr;
    c_mxid      choices;        // what each row of the targets or news view stands for
};

#endif
