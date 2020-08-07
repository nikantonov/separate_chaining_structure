#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

template <typename Key, size_t N = 7>
class ADS_set {
public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = key_type&;
    using const_reference = const key_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator;
    using const_iterator = Iterator;
    using key_compare = std::less<key_type>;   // B+-Tree
    using key_equal = std::equal_to<key_type>; // Hashing
    using hasher = std::hash<key_type>;// Hashing
private:
    size_type curr_size{0};
    size_type max_size{0};
    float max_lf{0.7};
    struct element{                   //element
        Key key;
        element* next{nullptr};
        ~element(){
            delete next;
        }
    };
    struct Netz{                 //Netz von ersten Elementen(heads)
        element* head{nullptr};
        ~Netz(){
            delete head;
        }
    };
    
    Netz * table = nullptr;
    
    size_type hash_idx(const key_type &key) const { return hasher{}(key) % max_size; }
    
    void insert_unchecked(const key_type &key);
    void rehash(size_type n);
    //void reserve(size_type n);
public:
    ADS_set(){ rehash(N); }
    ADS_set(initializer_list<key_type> ilist):ADS_set() { insert(ilist); }
    template<typename InputIt> ADS_set(InputIt first, InputIt last):ADS_set() { insert(first, last);  }
    ADS_set(const ADS_set& other):ADS_set() {
        for (const auto&key: other) insert_unchecked(key);
    }
    ~ADS_set() { delete [] table; }
    
    
    ADS_set& operator=(const ADS_set& other) {
        if (this == &other) return *this;
        ADS_set tmp{other};
        swap(tmp);
        return *this;
    }
    
    ADS_set& operator=(std::initializer_list<key_type> ilist) {
        ADS_set tmp{ilist};
        swap(tmp);
        return *this;
    }
    
    size_type size() const {return curr_size;};
    bool empty() const { return !curr_size; };
    
    size_type count(const key_type&) const;
    
    iterator find(const key_type& key) const {
        element* pos = nullptr;
        if (count(key) != 0){
            size_type idx = hash_idx(key);
            pos = table[idx].head;
            while(pos != nullptr){
                if(key_equal{}(pos->key, key)){
                    return iterator{table, pos, idx, max_size};}
                pos = pos->next;
            }
        }
        return end();
    }
    
    
    
    void clear() {
        delete [] table;
        curr_size = 0;
        max_size = 1;
        table = new Netz[max_size];
    }
    void swap(ADS_set& other) {
        std::swap(table, other.table);
        std::swap(curr_size, other.curr_size);
        std::swap(max_size, other.max_size);
        std::swap(max_lf, other.max_lf);
    }
    
    void insert(initializer_list<key_type> ilist);
    
    std::pair<iterator,bool> insert(const key_type& key){
        element* pos = nullptr;
        if (count(key) != 0){
            size_type idx = hash_idx(key);
            pos = table[idx].head;
            while(pos != nullptr){
                if(key_equal{}(pos->key, key)) return std::make_pair(const_iterator{table, pos, idx, max_size}, false);
                pos = pos->next;
            }
        }
        insert_unchecked(key);
      //  if (count(key) != 0){
            size_type idx = hash_idx(key);
            pos = table[idx].head;
            while(pos != nullptr){
                if(key_equal{}(pos->key, key)) return std::make_pair(const_iterator{table, pos, idx, max_size}, true);
                pos = pos->next;
            }
       // }
        return std::make_pair(end(),true);
        
    }
    
    /*std::pair<iterator,bool> insert(const key_type& key){
     if (count(key) != 0){
     return std::make_pair(const_iterator{*this,}, false);
     }
     
     insert_unchecked(key);
     return std::make_pair(, true);
     
     }*/
    
    //if(key_equal{}(runner->key, key))

    
    
    
    template<typename InputIt> void insert(InputIt first, InputIt last);
    
    size_type erase(const key_type& key) {
        if(count(key) != 0){
            size_type idx = hash_idx(key);
            if (key_equal{}(table[idx].head->key, key)){
                if (table[idx].head->next == nullptr){
                    element* runner = table[idx].head;
                    table[idx].head = nullptr;
                    delete runner;
                    --curr_size;
                    return 1;
                }else{
                    element* runner = table[idx].head->next;
                    element* deleter = table[idx].head;
                    deleter->next = nullptr;
                    delete deleter;
                    table[idx].head = runner;
                    runner = nullptr;
                    delete runner;
                    --curr_size;
                    return 1;
                }
            }else{
                element* runner = table[idx].head->next;
                element* old_runner = table[idx].head;
                do{
                    if(key_equal{}(runner->key, key) && runner->next == nullptr){
                        old_runner->next = nullptr;
                        runner->next = nullptr;
                        old_runner = nullptr;
                        delete old_runner;
                        delete runner;
                        --curr_size;
                        return 1;
                    }
                    
                    
                    if(key_equal{}(runner->key, key) && runner->next != nullptr){
                        old_runner->next = runner->next;
                        runner->next = nullptr;
                        old_runner = nullptr;
                        delete old_runner;
                        delete runner;
                        --curr_size;
                        return 1;
                    }
                    
                    old_runner = runner;
                    runner = runner->next;
                }while(runner != nullptr);
            }
        }
        return 0;
    }
   
    
    
    const_iterator begin() const {
        if(curr_size == 0) return end();
        size_type index = 0;
        while(table[index].head == nullptr) index++;
        return const_iterator{table, table[index].head, index, max_size}; }
    const_iterator end() const { return const_iterator{nullptr};}
    
    void dump(std::ostream& o = std::cerr) const;
    
    friend bool operator==(const ADS_set& lhs, const ADS_set& rhs) {
        if (lhs.curr_size != rhs.curr_size) return false;
        for(const auto& key: rhs){
            if (lhs.count(key) == 0) return false;
        }
        return true;
    }
    friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs) { return !(lhs == rhs); }
    
    //void inserter(size_type i){
     //  insert_unchecked(i);
    //}
    //std::less<key_type>{}(key1,key2)
    
    
    
};

template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
    Netz*table;
    //const ADS_set<Key,N>& as = ADS_set();           //wie vector& PRG
    element* pos;
    size_type index;
    size_type const_index = 0;
    size_type max_size {0};
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;
    
    
    explicit Iterator(Netz* table = nullptr, element* pos = nullptr,size_type index = 0, size_type max_size = 0):table{table}, pos{pos}, index{index}, max_size{max_size}{};
    
    ~Iterator(){
        table = nullptr;
        pos = nullptr;
        delete pos;
        delete [] table;
    }
    
    reference operator*() const{
        return pos->key;
    };
    
    pointer operator->() const{
        return &pos->key;
    };
    
    Iterator& operator++(){
        if (pos->next != nullptr){
            pos = pos->next;
            return *this;
        } else {if(index < max_size)index++;}
        
        if(table[index].head == nullptr){
            while(table[index].head == nullptr && index < max_size) index++;
        }
        if (index == max_size) {pos = nullptr; return *this;}
        pos = table[index].head;
        return *this;
    };
    Iterator operator++(int){
        Iterator rc{*this};
        ++*this;
        return rc;
    };
    friend bool operator==(const Iterator& lhs, const Iterator& rhs) { return lhs.pos == rhs.pos; }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return lhs.pos != rhs.pos; }
    
    
};


template <typename Key, size_t N> void swap(ADS_set<Key,N>& lhs, ADS_set<Key,N>& rhs) { lhs.swap(rhs);}

template <typename Key, size_t N>
void ADS_set<Key,N>::insert_unchecked(const key_type &key) {
    if(count(key) == 0){
        rehash(curr_size + 1);
        size_type idx = hash_idx(key);
        element* help = new element();
        help->key = key;
        if(table[idx].head == nullptr){
            table[idx].head = help;
            curr_size++;
        } else {
            help->next = table[idx].head;
            table[idx].head = help;
            curr_size++;
        }
        help = nullptr;
        delete help;
    }
}

template <typename Key, size_t N>
void ADS_set<Key,N>::rehash(size_type n){
    if(n > max_size * max_lf){
        Netz* old_table{nullptr};
        old_table = table;
        size_type old_max_size = max_size;
        if(max_size < 7){max_size = 7;}else{max_size = max_size*2 + 1;}
        table = new Netz[max_size];
        for(size_type i{0}; i < old_max_size; ++i){
            if(old_table[i].head != nullptr) {
                auto runner = old_table[i].head;
                do{
                    insert_unchecked(runner->key);
                    runner = runner->next;
                    curr_size--;
                }while(runner != nullptr);
                runner = nullptr;
                delete runner;
            }
        }
        delete [] old_table;
    }
}

template <typename Key, size_t N>
typename ADS_set<Key,N>::size_type ADS_set<Key,N>::count(const key_type& key) const {
    size_type idx = hash_idx(key);
    element* pos = nullptr;
    pos = table[idx].head;
    while (pos != nullptr){if(key_equal{}(pos->key, key)){return 1;} pos = pos->next;}
    return 0;
}

template <typename Key, size_t N>
void ADS_set<Key,N>::insert(initializer_list<key_type> ilist) {
    for (const auto& k: ilist)
        insert_unchecked(k);
}

template <typename Key, size_t N>
template <typename InputIt> void ADS_set<Key,N>::insert(InputIt first, InputIt last) {
    for (auto it = first; it != last; ++it) {
        
        insert_unchecked(*it);
    }
}

template <typename Key, size_t N>
void ADS_set<Key,N>::dump(ostream& o) const {
    for(size_type i{0}; i < max_size; ++i){
        o << i << ": ";
        if(table[i].head != nullptr) {
            auto runner = table[i].head;
            while(runner != nullptr){
                o << runner->key << " - ";
                runner = runner->next;
            }
            o << endl;
            runner= nullptr;
            delete runner;
        }
        else {
            o << " + "<< endl;
        }
    }
}














#endif // ADS_SET_H