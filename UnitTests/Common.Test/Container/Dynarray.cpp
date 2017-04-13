#include "stdafx.h"
#include "CppUnitTest.h"
#include "Common/Container/Dynarray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace CommonTest
{
    TEST_CLASS(Dynarray_Test)
    {

    public:

        TEST_METHOD(TestDynarray_indexing) {
            Dynarray<uint8>::size_type kItemsCount = 10;
            Dynarray<uint8> d(kItemsCount, 1);
            Assert::AreEqual(kItemsCount, d.size());
            for (int i = 0; i < (int)kItemsCount; ++i) Assert::AreEqual((uint8)1, d[i]);
        }

        TEST_METHOD(TestDynarray_forwardIterator) {
            Dynarray<uint8>::size_type kItemsCount = 10;
            Dynarray<uint8> d(kItemsCount, 1);
            iota(begin(d), end(d), 0);
            for (int i = 0; i < (int)kItemsCount; ++i) Assert::AreEqual((uint8)i, d[i]);
        }

        TEST_METHOD(TestDynarray_reverseIterator) {
            Dynarray<uint8>::size_type kItemsCount = 10;
            Dynarray<uint8> d(kItemsCount, 1);
            iota(rbegin(d), rend(d), 0);
            for (int i = 0; i < (int)kItemsCount; ++i) Assert::AreEqual((uint8)(kItemsCount - i - 1), d[i]);
        }

        TEST_METHOD(TestDynarray_frontBack) {
            Dynarray<uint8>::size_type kItemsCount = 2;
            Dynarray<uint8> d(kItemsCount, 1);
            d[0] = 1; d[1] = 2;
            Assert::AreEqual(d[0], d.front());
            Assert::AreEqual(d[1], d.back());
        }

        TEST_METHOD(TestDynarray_fill) {
            Dynarray<uint8>::size_type kItemsCount = 10;
            Dynarray<uint8> d(kItemsCount, 1);
            d.fill(10);
            for (int i = 0; i < (int)kItemsCount; ++i) Assert::AreEqual(d[i], (uint8)10);
        }

        TEST_METHOD(TestDynarray_at) {
            Dynarray<uint8>::size_type kItemsCount = 10;
            Dynarray<uint8> d(kItemsCount);
            Assert::AreEqual(d[kItemsCount - 1], d.at(kItemsCount - 1));
            Assert::ExpectException<out_of_range>([&]() { d.at(kItemsCount); });
        }

        TEST_METHOD(TestDynarray_data) {
            Dynarray<uint8>::size_type kItemsCount = 10;
            Dynarray<uint8> d(kItemsCount);
            iota(begin(d), end(d), 0);
            uint8* data = d.data();
            for (int i = 0; i < (int)kItemsCount; ++i) Assert::AreEqual(d[i], data[i]);
        }

        TEST_METHOD(TestDynarray_move) {
            Dynarray<uint8>::size_type kItemsCount = 10;
            Dynarray<uint8> to(Dynarray<uint8>(kItemsCount, 5));
            for (int i = 0; i < (int)kItemsCount; ++i) Assert::AreEqual(to[i], (uint8)5);
        }

    };
}