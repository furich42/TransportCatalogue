
#include "json_builder.h"


using namespace json;



Builder& Builder::Value(Node value) {
    if(end_builder) {
        throw std::logic_error("value builder");
    }
    
    if(!key_val && root_.IsDict()) {
        throw std::logic_error("Key_value problem");
    }
    
    if( root_ == nullptr  || root_.IsArray() || root_.IsDict()) {
    
    Node* nd = new Node;
    
    if(value.IsString()) {
        *nd = value.AsString();
    }
    if(value.IsInt()) {
        *nd = value.AsInt();
    }
    if(value.IsDouble()) {
        *nd = value.AsDouble();
    }
    if(value.IsBool()) {
        *nd = value.AsBool();
    }
    if(value.IsNull()) {
        *nd = Node{nullptr};
    }
    if(value.IsArray()) {
        *nd = value.AsArray();
    }
    if(value.IsDict()) {
        *nd = value.AsDict();
    }
    
    if(root_ == nullptr) {
        end_builder = true;
        root_ = *nd;
    } else {
        nodes_stack_.push_back(nd);
    }
    key_val = false;
    return *this;
    
    } else {
        throw std::logic_error("Value err");
    }
    
}

DictItemContext Builder::StartDict() {
    if(end_builder) {
        throw std::logic_error("value builder");
    }
    if(dicti == true) {
        throw std::logic_error("dicti err");
    }
    try {
    key_val = false;
    if(root_ == nullptr) {
        Dict dic;
        root_ = dic;
        working = true;
        dicti = true;
        DictItemContext dc(*this);
        return dc;
    } else {
        Builder* bd = new Builder;
        bd->prev = this;
        Dict dic;
        bd->root_ = dic;
        working = true;
        bd->dicti = true;
        bd->working = true;
        DictItemContext dc(*bd);
        return dc;
    }
    } catch(...) {
        throw std::logic_error("Start dict err");
    }
    
}

KeyItemContext Builder::Key(Node value){
    dicti = false;
    if(!root_.IsDict() || key_val == true || working == false) {
        throw std::logic_error("Key calld not in dict");
    }
    try {
    Node* nd = new Node;
    

    *nd = value.AsString();
    nodes_stack_.push_back(nd);
    key_val = true;
    KeyItemContext kic(*this);
    return kic;
    } catch(...) {
        throw std::logic_error("Key err");
    }
    
}

Builder& Builder::EndDict() {
    dicti = false;
    if(!root_.IsDict() || working == false) {
        throw std::logic_error("not Dict");
        
    }
    try{

    Dict dic;

    for(size_t i = 0; i < nodes_stack_.size();) {
        dic[(nodes_stack_.at(i))->AsString()] = *(nodes_stack_.at(i + 1));

        i+=2;
    }

    nodes_stack_.clear();
    root_ = dic;
    end_builder = true;
    if(prev == nullptr) {

        working = false;
        
        return *this;
    } else {
        (prev->nodes_stack_).push_back(&root_);
        
        prev->working = false;
        if((prev->root_).IsArray() || (prev->root_).IsDict()) {
            prev->working = true;
        }
        return *prev;
    }
    } catch(...) {
        throw std::logic_error("end dict err");
    }
}


ArrayItemContext Builder::StartArray() {
    if(end_builder) {
        throw std::logic_error("value builder");
    }
    try{
    key_val = false;
    
    if(root_ == nullptr) {
        Array arr;
        root_ = arr;
        working = true;
        ArrayItemContext aic(*this);
        return aic;
    } else {
        Builder* bd = new Builder;
        bd->prev = this;
        Array arr;
        bd->root_ = arr;
        bd->working = true;
        working = true;
        ArrayItemContext aic(*bd);
        return aic;
    }
    
    } catch(...) {
        throw std::logic_error("start arr err");
    }
}

Builder& Builder::EndArray() {
    if(!root_.IsArray() || working == false) {
        throw std::logic_error("not array");
    }
    try{
    Array arr;
    
    for(auto n : nodes_stack_) {
        arr.push_back(*n);
        root_ = arr;
    }
    nodes_stack_.clear();
    end_builder = true;
    if(prev == nullptr) {
        working = false;
        
        return *this;
    } else {
        (prev->nodes_stack_).push_back(&root_);
        prev->working = false;
        if((prev->root_).IsArray() || (prev->root_).IsDict()) {
            prev->working = true;
        }
        return *prev;
    }
    } catch(...) {
        throw std::logic_error("end arr err");
    }
    
}


Node Builder::Build() {
    if(nodes_stack_.size() != 0 || working == true || key_val == true ){
        throw std::logic_error("build err");
    }
    if(root_.IsString()) {
        return root_.AsString();
    } else if(root_.IsArray()) {
        return root_.AsArray();
    } else if(root_.IsDict()) {
        return root_.AsDict();
    } else if(root_.IsInt()) {
        return root_.AsInt();
    } else if(root_.IsDouble()) {
        return root_.AsDouble();
    } else if(root_.IsBool()) {
        return root_.AsBool();
    } else {
        
        throw std::logic_error("build err");
    }
    
    
    
    
    
}

    Builder& DictItemContext::EndDict() {
        return builder_.EndDict();
    }
    
    KeyItemContext DictItemContext::Key(Node value) {
        return builder_.Key(value);
    }

    KeyValueItemContext KeyItemContext::Value(Node value){
        return builder_.Value(value);
    }
    
    DictItemContext KeyItemContext::StartDict() {
        return builder_.StartDict();
    }

    
    ArrayItemContext KeyItemContext::StartArray() {
        return builder_.StartArray();
    }

    KeyItemContext KeyValueItemContext::Key(Node value) {
        return builder_.Key(value);
    }

    Builder& KeyValueItemContext::EndDict() {
        return builder_.EndDict();
    }


    ArrayValueItemContext ArrayItemContext::Value(Node value) {
        return builder_.Value(value);
    }
    
    DictItemContext ArrayItemContext::StartDict() {
        return builder_.StartDict();
    }
    
    ArrayItemContext ArrayItemContext::StartArray() {
        return builder_.StartArray();
    }
    
    Builder& ArrayItemContext::EndArray() {
        return builder_.EndArray();
    }

    ArrayValueItemContext ArrayValueItemContext::Value(Node value) {
        return builder_.Value(value);
    }
    
    DictItemContext ArrayValueItemContext::StartDict() {
        return builder_.StartDict();
    }
    
    ArrayItemContext ArrayValueItemContext::StartArray() {
        return builder_.StartArray();
    }
    
    Builder& ArrayValueItemContext::EndArray() {
        return builder_.EndArray();
    }



