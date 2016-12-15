#include "stdafx.h"

#include "Menu/Menu.h"

#include "Pile/Pattern/Command.h"

using namespace Pile;

namespace Viewer
{
    Menu::~Menu()
    {
        for (uint32 i = 0; i < m_Items.size(); i++)
        {
            if (m_Items[i].m_Command != NULL)
                delete m_Items[i].m_Command;
        }
    }

    int_t Menu::AddNode(const int_t parentId, const char* title)
    {
        const uint_t itemId = m_Items.size();
        m_Items.push_back({ NULL, title });
        m_Hierarchy.insert(Pair(parentId, itemId));
        return itemId;
    }

    void Menu::AddLeaf(const int_t parentId, Command *command, const char* title)
    {
        m_Items.push_back({ command, title });
        m_Hierarchy.insert(Pair(parentId, m_Items.size() - 1));
    }

    const Menu::Item& Menu::GetItem(const int_t id) const
    {
        return m_Items[id];
    }

    bool Menu::IsLeaf(const int_t id) const
    {
        std::pair<LevelIterator, LevelIterator> level;
        level = m_Hierarchy.equal_range(id);

        return level.first == level.second;

    }

    Menu::LevelRange Menu::GetLevelRange(const int_t parentId) const
    {
        std::pair<LevelIterator, LevelIterator> level;
        level = m_Hierarchy.equal_range(parentId);
        LevelRange levelRange(level.first, level.second);

        return levelRange;
    }
}
