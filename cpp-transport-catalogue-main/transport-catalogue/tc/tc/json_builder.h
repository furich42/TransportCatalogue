#pragma once
#include "json.h"
#include <string>


namespace json {
    
class DictItemContext;
class KeyItemContext;
class ArrayItemContext;
class ArrayValueItemContext;
class KeyValueItemContext;

class Builder {
public:
    
    
    Builder() = default;
    
    
    
    Builder& Value(Node value);
    
    DictItemContext StartDict();
    Builder& EndDict();
    
    ArrayItemContext StartArray();
    Builder& EndArray();
    
    KeyItemContext Key(Node value);
    
    Node Build();
    
    ~Builder() = default;
    
protected:
    Node root_ = {};
    Builder* prev;
    std::vector<Node*> nodes_stack_;
    bool working = false;
    bool key_val = false;
    bool end_builder = false;
    bool dicti = false;
    
    
};
    
class DictItemContext{
public:
    
    DictItemContext(Builder& builder)
        :builder_(builder){
        
    }
    Builder& EndDict();
    
    KeyItemContext Key(Node value);
    
    Builder& Value(Node value) = delete;
    
    DictItemContext StartDict() = delete;

    
    Builder& StartArray() = delete;
    Builder& EndArray() = delete;
    

    
    Node Build() = delete;
    
private:
    Builder& builder_;
    
    
};
    
    
class KeyItemContext {
public:
    
    KeyItemContext(Builder& builder)
        :builder_(builder){
        
    }
    
    
    KeyValueItemContext Value(Node value);
    
    DictItemContext StartDict();

    
    ArrayItemContext StartArray();
    
    
private:
    Builder& builder_;
    
    
};
    
    
class KeyValueItemContext {
public:
    
    KeyValueItemContext(Builder& builder)
        :builder_(builder){
        
    }
    
    Builder& EndDict();
    
    KeyItemContext Key(Node value);
    
    
private:
    Builder& builder_;
    
    
};
    
class ArrayItemContext {
public:
    
    ArrayItemContext(Builder& builder)
        :builder_(builder){
        
    }
    
    ArrayValueItemContext Value(Node value);
    
    DictItemContext StartDict();
    
    ArrayItemContext StartArray();
    
    Builder& EndArray();
    
    
    
    
private:
    Builder& builder_;
    
    
};
    
class ArrayValueItemContext {
public:
    
    ArrayValueItemContext(Builder& builder)
        :builder_(builder){
        
    }
    
    ArrayValueItemContext Value(Node value);
    
    DictItemContext StartDict();
    
    ArrayItemContext StartArray();
    
    Builder& EndArray();
    
    
    
    
private:
    Builder& builder_;
    
    
};
    

    
    
}
