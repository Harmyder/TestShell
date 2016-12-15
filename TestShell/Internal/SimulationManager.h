#ifndef __SIMULATION_MANAGER__
#define __SIMULATION_MANAGER__

class BaseFactory;

class SimulationManager
{
public:
    ~SimulationManager();
    void Init();
    void Close();

    uint32       GetFactoriesCount() const;
    BaseFactory *GetFactory(uint32 i) const;

private:
    void RegisterSimulations();
    void UnregisterSimulations();

private:
    void RegisterFactory(BaseFactory *factory);

private:
    std::vector<BaseFactory*> *factories_;
};

#endif // __SIMULATION_MANAGER__
