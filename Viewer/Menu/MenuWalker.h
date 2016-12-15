#pragma once

namespace Viewer
{
    class Menu;

    class MenuWalker
    {
    public:
        MenuWalker(const Menu *menu);
        const Menu* GetMenu() const;

        void GoChild();
        void GoParent();

        void GoNextHandle();
        void GoPrevious();

        typedef std::pair<uint32, const char *> VisibleItem;
        void GetVisibleItems(std::vector<VisibleItem> &items, uint_t &selectedItem);

    private:
        int GetVisibleItemsInternal(std::vector<VisibleItem> &items, const uint_t level);

        void GoSibling(bool isForward);

    private:
        const Menu *m_Menu;
        std::vector<int_t> m_Selection;
    };
}
