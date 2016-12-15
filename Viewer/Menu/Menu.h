#pragma once

#include "Pile/Attribute/NonCopyable.h"

namespace Pile {
    class Command;
}

namespace Viewer
{
    class Menu : public Pile::NonCopyable
    {
    public:
        ~Menu();

        int_t AddNode(const int_t parentId, const char* title);
        void  AddLeaf(const int_t parentId, Pile::Command *command, const char* title);

        struct Item
        {
            Pile::Command    *m_Command;
            const char *m_Title;
        };
        const Item& GetItem(const int_t id) const;
        bool        IsLeaf(const int_t id) const;

        typedef std::multimap<int_t, int_t>::const_iterator LevelIterator;
        typedef std::pair<LevelIterator, LevelIterator> LevelRange;

        LevelRange GetLevelRange(const int_t parentId) const;

        static const int_t ROOT_ID = -1;

    private:
        typedef std::pair<int_t, int_t> Pair;
        std::multimap<int_t, int_t>  m_Hierarchy;
        std::vector<Item> m_Items;
    };
}
