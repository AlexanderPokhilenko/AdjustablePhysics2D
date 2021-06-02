#ifndef ADJUSTABLEPHYSICS2D_DOUBLEKEYCONTAINERTEMPLATESIMPL_H
#define ADJUSTABLEPHYSICS2D_DOUBLEKEYCONTAINERTEMPLATESIMPL_H

template<typename TKey, typename TValue>
DoubleKeyContainer<TKey, TValue>::DoubleKeyContainer(std::size_t capacity) : maps() {
    maps.reserve(capacity);
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::forEach(const std::function<void(TValue &)>& func) {
    auto size = values.size();
    for (size_t i = 0; i < size; i++) {
        if(freeIndexes.find(i) != freeIndexes.end()) continue;
        func(values[i]);
    }
}

template<typename TKey, typename TValue>
std::vector<typename std::vector<TValue>::iterator> DoubleKeyContainer<TKey, TValue>::get(TKey id1, TKey id2) {
    std::vector<typename std::vector<TValue>::iterator> vector{};
    for (auto &index : maps[id1][id2]) vector.insert(std::advance(values.begin(), index));
    return vector;
}

template<typename TKey, typename TValue>
const std::vector<typename std::vector<TValue>::const_iterator> DoubleKeyContainer<TKey, TValue>::get(TKey id1, TKey id2) const {
    const std::vector<typename std::vector<TValue>::const_iterator> vector{};
    for (auto &index : maps[id1][id2]) vector.insert(std::advance(values.cbegin(), index));
    return vector;
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::remove(TKey id1, TKey id2) {
    for (auto &index : maps[id1][id2]) freeIndexes.insert(index);
    maps[id1].erase(id2);
    maps[id2].erase(id1);
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::removeAll(TKey id) {
    for (auto &pair : maps[id]) {
        for (auto &index : maps[pair.first][id]) freeIndexes.insert(index);
        maps[pair.first].erase(id);
    }
    maps.erase(id);
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::tryRemoveAll(TKey id) {
    if(maps.find(id) == maps.end()) return;
    removeAll(id);
}

template<typename TKey, typename TValue>
template<typename... Args>
void DoubleKeyContainer<TKey, TValue>::add(TKey id1, TKey id2, Args &&... args) {
    size_t index;
    if(freeIndexes.empty()) {
        index = values.size();
        values.emplace_back(args...);
    } else {
        auto indexIterator = freeIndexes.begin();
        index = *indexIterator;
        values[index] = TValue(args...);
        freeIndexes.erase(indexIterator);
    }
    if(maps.find(id1) == maps.end()) maps.insert({id1, std::unordered_map<TKey, std::unordered_set<size_t>>()});
    if(maps[id1].find(id2) == maps[id1].end()) maps[id1].insert({id2, std::unordered_set<size_t>()});
    maps[id1][id2].insert(index);
    if(maps.find(id2) == maps.end()) maps.insert({id2, std::unordered_map<TKey, std::unordered_set<size_t>>()});
    if(maps[id2].find(id1) == maps[id2].end()) maps[id2].insert({id1, std::unordered_set<size_t>()});
    maps[id2][id1].insert(index);
}

template <typename TValue, typename = void>
struct ValueHasIds : std::false_type {};

template <typename TValue>
struct ValueHasIds<TValue, decltype((void)TValue::id1, (void)TValue::id2, void())> : std::true_type {};

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::remove(typename std::vector<TValue>::iterator iterator, std::false_type) {
    auto index = std::distance(values.begin(), iterator);
    for (auto &pair1 : maps) {
        for (auto &pair2 : pair1.second) {
            auto setIt = pair2.second.find(index);
            if(setIt != pair2.second.end()) {
                pair2.second.erase(setIt);
                maps[pair2.first][pair1.first].erase(index);
                freeIndexes.insert(index);
                return;
            }
        }
    }
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::remove(typename std::vector<TValue>::iterator iterator, std::true_type) {
    auto index = std::distance(values.begin(), iterator);
    maps[(*iterator).id1][(*iterator).id2].erase(index);
    maps[(*iterator).id2][(*iterator).id1].erase(index);
    freeIndexes.insert(index);
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::remove(typename std::vector<TValue>::const_iterator iterator, std::false_type) {
    auto index = std::distance(values.cbegin(), iterator);
    for (auto &pair1 : maps) {
        for (auto &pair2 : pair1.second) {
            auto setIt = pair2.second.find(index);
            if(setIt != pair2.second.end()) {
                pair2.second.erase(setIt);
                maps[pair2.first][pair1.first].erase(index);
                freeIndexes.insert(index);
                return;
            }
        }
    }
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::remove(typename std::vector<TValue>::const_iterator iterator, std::true_type) {
    auto index = std::distance(values.cbegin(), iterator);
    maps[(*iterator).id1][(*iterator).id2].erase(index);
    maps[(*iterator).id2][(*iterator).id1].erase(index);
    freeIndexes.insert(index);
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::remove(typename std::vector<TValue>::iterator iterator) {
    return remove(iterator, ValueHasIds<TValue>{});
}

template<typename TKey, typename TValue>
void DoubleKeyContainer<TKey, TValue>::remove(typename std::vector<TValue>::const_iterator iterator) {
    return remove(iterator, ValueHasIds<TValue>{});
}

#endif //ADJUSTABLEPHYSICS2D_DOUBLEKEYCONTAINERTEMPLATESIMPL_H
