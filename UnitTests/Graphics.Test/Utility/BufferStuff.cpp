#include "stdafx.h"
#include "CppUnitTest.h"
#include "Graphics\Utility\BufferStuff.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GraphicsTest
{
    TEST_CLASS(BufferStuff)
    {
    public:

        TEST_METHOD(TestFreeIndices_FirstTakenSimple) {
            Graphics::Utility::FreeIndices fi(10);
            auto index = fi.AcquireIndex();
            Assert::AreEqual(index, fi.FirstTakenIndex());
        }

        TEST_METHOD(TestFreeIndices_FirstTakenPristine) {
            Graphics::Utility::FreeIndices fi(10);
            Assert::AreEqual((uint32)10, fi.FirstTakenIndex());
        }

        TEST_METHOD(TestFreeIndices_AcquireAll) {
            constexpr uint32 kIndices = 10;
            Graphics::Utility::FreeIndices fi(kIndices);

            for (uint32 i = 0; i < kIndices; ++i) fi.AcquireIndex();

            Assert::AreEqual((uint32)0, fi.FirstTakenIndex());
            for (uint32 i = 1; i <= kIndices; ++i)
                Assert::AreEqual(i, fi.NextTakenIndex(i - 1));
        }

        TEST_METHOD(TestFreeIndices_SeveralTakenInRow) {
            constexpr uint32 kIndices = 10;
            constexpr uint32 kAcquiredCount = 5;
            Graphics::Utility::FreeIndices fi(kIndices);

            for (uint32 i = 0; i < kAcquiredCount; ++i) fi.AcquireIndex();

            uint32 index = fi.FirstTakenIndex();
            uint32 takenCount = 0;
            while (index != kIndices) {
                index = fi.NextTakenIndex(index);
                ++takenCount;
            }

            Assert::AreEqual(kAcquiredCount, takenCount);
        }

        TEST_METHOD(TestFreeIndices_AcquireAllReleaseOne) {
            constexpr uint32 kIndices = 10;
            constexpr uint32 kToRelease = 5;
            Graphics::Utility::FreeIndices fi(kIndices);

            for (uint32 i = 0; i < kIndices; ++i) fi.AcquireIndex();

            fi.ReleaseIndex(kToRelease);

            Assert::AreEqual(kToRelease + 1, fi.NextTakenIndex(kToRelease));
        }

        TEST_METHOD(TestFreeIndices_AcquireAllReleaseTwo) {
            constexpr uint32 kIndices = 10;
            constexpr uint32 kToRelease = 5;
            Graphics::Utility::FreeIndices fi(kIndices);

            for (uint32 i = 0; i < kIndices; ++i) fi.AcquireIndex();

            fi.ReleaseIndex(kToRelease);
            fi.ReleaseIndex(kToRelease + 1);

            set<uint32> s;
            for (uint32 i = 0; i < kIndices; ++i) s.insert(i);
            s.erase(kToRelease);
            s.erase(kToRelease + 1);

            uint32 index = fi.FirstTakenIndex();
            while (index < kIndices) {
                Assert::IsTrue(s.find(index) != end(s));
                s.erase(index);
                index = fi.NextTakenIndex(index);
            }
            Assert::AreEqual(s.size(), (size_t)0);
        }
    };
}