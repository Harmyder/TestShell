#include "stdafx.h"

#include "Menu/MenuWalker.h"
#include "Menu/Menu.h"
#include "Pile/Pattern/Command.h"

using namespace std;
using namespace Pile;

namespace Viewer
{
    MenuWalker::MenuWalker(const Menu *menu) :
        m_Menu(menu)
    {
        m_Selection.push_back(Menu::ROOT_ID);
    }

    void MenuWalker::GoChild()
    {
        const int_t selectedItem = m_Selection.back();
        Menu::LevelRange levelRange = m_Menu->GetLevelRange(selectedItem);
        if (levelRange.first != levelRange.second)
        {
            Menu::LevelIterator it = levelRange.first;
            const int_t childItemId = (*it).second;
            m_Selection.push_back(childItemId);
        }
        else
        {
            Command* command = m_Menu->GetItem(selectedItem).m_Command;
            command->Execute();
        }
    }

    void MenuWalker::GoParent()
    {
        if (m_Selection.back() != Menu::ROOT_ID)
            m_Selection.pop_back();
    }

    void MenuWalker::GoNextHandle()
    {
        GoSibling(true);
    }

    void MenuWalker::GoPrevious()
    {
        GoSibling(false);
    }

    void MenuWalker::GetVisibleItems(std::vector<VisibleItem> &items, uint_t &selectedItem)
    {
        assert(items.size() == 0);
        GetVisibleItemsInternal(items, 0);
        selectedItem = m_Selection.back();
    }

    int_t MenuWalker::GetVisibleItemsInternal(std::vector<VisibleItem> &items, const uint_t level)
    {
        int_t result = 0; // Select the root item by default
        Menu::LevelRange levelRange = m_Menu->GetLevelRange(m_Selection[level]);

        // Previously inserted item is the selected one
        if (levelRange.first == levelRange.second)
        {
            result = items.size() - 1;
        }
        else
        {
            Menu::LevelIterator it;
            const int_t levelSelectedItemId = m_Selection.size() > (level + 1) ? m_Selection[level + 1] : -1;
            for (it = levelRange.first; it != levelRange.second; it++)
            {
                const int_t levelItemId = (*it).second;
                items.push_back(VisibleItem(level, m_Menu->GetItem(levelItemId).m_Title));

                if (levelSelectedItemId == levelItemId)
                {
                    result = GetVisibleItemsInternal(items, level + 1);
                }
            }
        }
        return result;
    }

    void MenuWalker::GoSibling(bool isForward)
    {
        const int_t selectedItem = m_Selection.back();
        if (selectedItem != Menu::ROOT_ID)
        {
            const int_t parentId = *(m_Selection.end() - 2);

            // We are surely have at least one child item
            Menu::LevelRange levelRange = m_Menu->GetLevelRange(parentId);
            Menu::LevelIterator it;
            for (it = levelRange.first; it != levelRange.second; it++)
            {
                const int_t childItemId = (*it).second;
                if (childItemId == selectedItem)
                {
                    if (isForward)
                    {
                        it++;
                        if (it == levelRange.second)
                            it = levelRange.first;
                    }
                    else
                    {
                        if (it == levelRange.first)
                            it = levelRange.second;
                        it--;
                    }
                    break;
                }
            }
            m_Selection.back() = ((*it).second);
        }
    }
}
