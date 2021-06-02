#ifndef ADJUSTABLEPHYSICS2D_DOUBLEKEYCONTAINER_H
#define ADJUSTABLEPHYSICS2D_DOUBLEKEYCONTAINER_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <type_traits>

template<typename TKey, typename TValue>
class DoubleKeyContainer {
private:
    std::unordered_map<TKey, std::unordered_map<TKey, std::unordered_set<size_t>>> maps;
    std::vector<TValue> values;
    std::unordered_set<size_t> freeIndexes;
    void remove(typename std::vector<TValue>::iterator iterator, std::false_type);
    void remove(typename std::vector<TValue>::iterator iterator, std::true_type);
    void remove(typename std::vector<TValue>::const_iterator iterator, std::false_type);
    void remove(typename std::vector<TValue>::const_iterator iterator, std::true_type);
public:
    explicit DoubleKeyContainer(std::size_t capacity = 16);
    void forEach(const std::function<void(TValue&)>&);
    std::vector<typename std::vector<TValue>::iterator> get(TKey id1, TKey id2);
    const std::vector<typename std::vector<TValue>::const_iterator> get(TKey id1, TKey id2) const;
    void remove(typename std::vector<TValue>::iterator iterator);
    void remove(typename std::vector<TValue>::const_iterator iterator);
    void remove(TKey id1, TKey id2);
    void removeAll(TKey id);
    void tryRemoveAll(TKey id);
    template<typename... Args>
    void add(TKey id1, TKey id2, Args&&... args);
};

#include "DoubleKeyContainerTemplatesImpl.h"
#endif //ADJUSTABLEPHYSICS2D_DOUBLEKEYCONTAINER_H
