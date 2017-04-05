#include "stdafx.h"
#include "CppUnitTest.h"
#include "Math/Graph/__utils.h"
#include "Common/Math/Graph/EdmondsKarp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace CommonTest
{
    TEST_CLASS(EdmondsKarp_Test)
    {
    public:

        TEST_METHOD(TestEdmondsKarp_Cycle) {
            const int verticesCount = 10;
            vector<int> edgesData(verticesCount);
            iota(begin(edgesData), end(edgesData), 1);
            const auto edgeData = [&edgesData](int from, int j) { return Capacity<int>(edgesData[from]); };
            struct Pred { bool operator()(const EdgeData<Capacity<int>>& d) { return d.edgeData.GetCapacity() > 0; }; };
            const auto g = CreateGraphCycle<Pred>(verticesCount, edgeData);

            EdmondsKarp<decltype(g)> ek(g, 0, verticesCount - 1);
            ek.Run();
            const auto flow = ek.CollectFlow();

            for (int i = 0; i < verticesCount - 2; ++i) Assert::AreEqual(1, flow[i][0]);
            Assert::AreEqual(0, flow[verticesCount - 1][0]);
        }
    };
}