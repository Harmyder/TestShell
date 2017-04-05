#pragma once
// Classes that represent graph structure with adjacency lists.
// Good fit for sparse graphs and in case of algorithms which exploit adjacency lists like BFS.

namespace Common
{
    template <class Myvec, class Pred, class NestedIterator, class pointer, class reference>
    class AdjacencyVectorBaseIterator
    {
    public:
        using IsAdjacencyVectorBaseIterator = std::true_type;
        using Myiter = AdjacencyVectorBaseIterator<Myvec, Pred, NestedIterator, pointer, reference>;
        using iterator_category = std::forward_iterator_tag;

        using value_type = typename Myvec::value_type;
        using difference_type = unsigned __int64;

        AdjacencyVectorBaseIterator(const Myvec& cont, NestedIterator iter) : cont_(cont), iter_(iter) {
            while (iter_ != cont_.end() && !Pred()(*iter_)) ++iter_;
        }

        reference operator*() const { return *iter_; }
        pointer operator->() const { return iter_.operator->(); }

        Myiter& operator++() {
            ++iter_;
            while (iter_ != cont_.end() && !Pred()(*iter_)) ++iter_;
            return (*this);
        }
        Myiter operator++(int) {
            Myiter tmp = *this;
            ++*this;
            return (tmp);
        }

        Myiter& operator+=(difference_type off) {
            for (difference_type i = 0; i < off; ++i)++*this;
            return (*this);
        }
        Myiter operator+(difference_type off) const {
            Myiter tmp = *this;
            return (tmp += off);
        }

        difference_type operator-(const Myiter& other) const { return iter_ - other.iter_; }

        bool operator==(const Myiter& right) const { return iter_ == right.iter_; }
        bool operator!=(const Myiter& right) const { return iter_ != right.iter_; }
        bool operator<(const Myiter& right) const { return iter_ < right.iter_; }
        bool operator>(const Myiter& right) const { return right < *this; }
        bool operator<=(const Myiter& right) const { return (!(right < *this)); }
        bool operator>=(const Myiter& right) const { return (!(*this < right)); }

    private:
        const Myvec& cont_;
        NestedIterator iter_;
    };

    template<class Myvec, class Pred>
    class AdjacencyVectorConstIterator : public AdjacencyVectorBaseIterator<Myvec, Pred, typename Myvec::const_iterator, typename Myvec::const_pointer, typename Myvec::const_reference>
    {
    public:
        using AdjacencyVectorBaseIterator<Myvec, Pred, typename Myvec::const_iterator, typename Myvec::const_pointer, typename Myvec::const_reference>::AdjacencyVectorBaseIterator;
    };

    template<class Myvec, class Pred>
    class AdjacencyVectorIterator : public AdjacencyVectorBaseIterator<Myvec, Pred, typename Myvec::iterator, typename Myvec::pointer, typename Myvec::reference>
    {
    public:
        using AdjacencyVectorBaseIterator<Myvec, Pred, typename Myvec::iterator, typename Myvec::pointer, typename Myvec::reference>::AdjacencyVectorBaseIterator;
    };

    template <class U>
    struct Capacity
    {
        explicit Capacity(U capacity) : capacity_(capacity) {}
        void SetCapacity(U capacity) { capacity_ = capacity; }
        U GetCapacity() const { return capacity_; }

        bool operator<(const Capacity& other) const { return capacity_ < other.capacity_; }

        using CapacityUnit = U;

    private:
        U capacity_;
    };

    template <class U>
    struct CapacityFlow
    {
        CapacityFlow(U capacity) : capacity_(capacity) {}

        U GetCapacity() { return capacity_; }
        U GetFlow() { return capacityResidual_; }
        U GetResidualCapacity() { return capacity_ - flow_; }

        void IncreaseFlow(U additionalFlow) {
            flow += additionalFlow;
            assert(0 <= flow_ && flow_ <= capacity_);
        }

    private:
        U capacity_;
        U flow_;
    };

    template <class U>
    struct Weight
    {
        U GetWeight() { return weight_; }
        void SetWeight(U weight) { weight_ = weight; }

    private:
        U weight_;
    };

    template <class... Attributes>
    class VertexData : public Attributes... {};

    class Graph {
    public:
        enum { kNoVertex = -1 };
    };

    template <class II, class ICI>
    class GraphBaseOfBase : public Graph {
    public:
        using IndexIterator = II;
        using IndexConstIterator = ICI;
        using AdjacencyDatum = typename IndexConstIterator::value_type;

        void InitAdjacency(int v, const std::vector<AdjacencyDatum>& adjacencyData) {
            assert(adjacentEdges_[v].size() == 0 && std::is_sorted(cbegin(adjacencyData), cend(adjacencyData), []()));
            adjacentEdges_[v] = adjacencyData;
        }
        void InitAdjacency(int v, std::vector<AdjacencyDatum>&& adjacencyData) {
            assert(adjacentEdges_[v].size() == 0 && std::is_sorted(cbegin(adjacencyData), cend(adjacencyData)));
            adjacentEdges_[v] = move(adjacencyData);
        }

        int GetVerticesCount() const { return (int)adjacentEdges_.size(); }

    protected:
        void InitAdjacentVerticesCount(int verticesCount) {
            assert(adjacentEdges_.size() == 0);
            adjacentEdges_.resize(verticesCount);
        }

        std::vector<std::vector<AdjacencyDatum>> adjacentEdges_;
    };

    template<class II, class ICI>
    class GraphBase : public GraphBaseOfBase<II, ICI>
    {
    public:
        std::pair<IndexConstIterator, IndexConstIterator> GetAdjacencyData(int i) const {
            return make_pair(cbegin(adjacentEdges_[i]), cend(adjacentEdges_[i]));
        }
        std::pair<IndexIterator, IndexIterator> GetAdjacencyDataNonConst(int i) {
            return make_pair(begin(adjacentEdges_[i]), end(adjacentEdges_[i]));
        }
    };

    template <class Myvec, class Pred>
    class GraphBase<
        AdjacencyVectorIterator<Myvec, Pred>,
        AdjacencyVectorConstIterator<Myvec, Pred>> :
        public GraphBaseOfBase<AdjacencyVectorIterator<Myvec, Pred>, AdjacencyVectorConstIterator<Myvec, Pred>>
    {
    public:
        std::pair<IndexConstIterator, IndexConstIterator> GetAdjacencyData(int i) const {
            return make_pair(IndexConstIterator(adjacentEdges_[i], cbegin(adjacentEdges_[i])), IndexConstIterator(adjacentEdges_[i], cend(adjacentEdges_[i])));
        }
        std::pair<IndexIterator, IndexIterator> GetAdjacencyDataNonConst(int i) {
            IndexIterator b(adjacentEdges_[i], begin(adjacentEdges_[i]));
            return make_pair(IndexIterator(adjacentEdges_[i], begin(adjacentEdges_[i])), IndexIterator(adjacentEdges_[i], end(adjacentEdges_[i])));
        }
    };

    template <class ED>
    struct EdgeData {
        EdgeData(int indexTo, const ED& edgeData) : indexTo(indexTo), edgeData(edgeData) {}
        EdgeData(int indexTo, ED&& edgeData) : indexTo(indexTo), edgeData(move(edgeData)) {}
        bool operator<(const EdgeData<ED>& other) const { return indexTo < other.indexTo; }
        using EdgeDataType = ED;
        int indexTo;
        ED edgeData;
    };

    // Here only getter for ajacent vertex index presented, it is expected that if edge data is present then the user knows about it
    template <class ED>
    inline int GetAdjacentIndex(const EdgeData<ED>& d) { return d.indexTo; }
    inline int GetAdjacentIndex(int d) { return d; }

    template <class ED>
    using AdjacencyContainer = std::vector<std::conditional_t<std::is_same<ED, void>::value, int, EdgeData<ED>>>;

    template <
        class ED,
        class VD,
        class II = typename AdjacencyContainer<ED>::iterator,
        class ICI = typename AdjacencyContainer<ED>::const_iterator>
    class DirectedGraph :
        public DirectedGraph<ED, class VertexData<>, ICI>,
        public DirectedGraph<void, VD, ICI>
    {
    public:
        void InitVertices(const std::vector<VD>& vertices) {
            InitAdjacentVerticesCount(verticesCount);
            vertices_ = vertices;
            adjacentEdgesData_.resize(verticesCount);
        }
        void InitVertices(std::vector<VD>&& vertices) {
            InitAdjacentVerticesCount(verticesCount);
            vertices_ = move(vertices);
            adjacentEdgesData_.resize(verticesCount);
        }
        void InitVertices(int verticesCount) = delete;
    };

    template <class ED, class II, class ICI>
    class DirectedGraph<ED, VertexData<>, II, ICI> : public virtual GraphBase<II, ICI> {
    public:
        void InitVertices(int verticesCount) {
            InitAdjacentVerticesCount(verticesCount);
        }
    };


    template <class VD, class II, class ICI>
    class DirectedGraph<void, VD, II, ICI> : public virtual GraphBase<II, ICI> {
    public:
        void InitVertices(const std::vector<VD>& vertices) {
            InitAdjacentVerticesCount(verticesCount);
            vertices_ = vertices;
        }
        void InitVertices(std::vector<VD>&& vertices) {
            InitAdjacentVerticesCount((int)vertices.size());
            vertices_ = move(vertices);
        }

        auto GetVerticesData() const { return make_pair(cbegin(vertices_), cend(vertices_)); }
        int GetVerticesCount() const { return (int)vertices_.size(); }

    private:
        std::vector<VD> vertices_;
    };

    // Can be useful for representing graph's layout
    template <class II, class ICI>
    class DirectedGraph<void, VertexData<>, II, ICI> : public GraphBase<II, ICI> {
    public:
        void InitVertices(int verticesCount) {
            InitAdjacentVerticesCount(verticesCount);
        }
    };
}