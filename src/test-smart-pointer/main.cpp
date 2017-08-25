//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <Windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <list>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>


class Object {
public:
    Object(int id): m_id(id) {
        std::cout << "Object()" << std::endl;
    }
    ~Object() {
        std::cout << "~Object()" << std::endl;
    }
    int id()const {
        return m_id;
    }
private:
    int m_id;
};
typedef boost::shared_ptr<Object> ObjectPtr;
typedef boost::weak_ptr<Object> WeakObjectPtr;
void deleterofObject(Object* pobj) {
    if (pobj) {
        std::cout << "delete obj" << pobj->id() << std::endl;
        delete pobj;
    }
}

void print(ObjectPtr obj) {
    std::cout << "count " << obj.use_count() << " id :" << obj->id() << std::endl;
}


void printRef(const ObjectPtr& obj) {
    std::cout << "ref count " << obj.use_count() << " id :" << obj->id() << std::endl;
}


void interfaceofSharedPtr() {
    ObjectPtr obj(new Object(1));
    std::cout << "ref count is " << obj.use_count() << std::endl;
    WeakObjectPtr weakObj(obj);
    WeakObjectPtr weakObj2(obj);
    print(obj);
    ObjectPtr p = weakObj.lock();
    if (p) {
        std::cout << p.unique() << std::endl;
    }

    printf("------------------------------\n");
    //值传入会改变引用计数（+1）
    print(obj);
    printRef(obj);

    printf("------------------------------\n");

    ObjectPtr obj2(obj);
    std::cout << "ref count is " << obj.use_count() << std::endl;
    obj2.reset();		//不要了，引用计数-1
    //obj2 = nullptr;
    std::cout << "ref count is " << obj.use_count() << std::endl;


}

int main(int argc, char** argv) {
    doctest::Context context;


    //context.addFilter("test-case-exclude", "*math*");

    context.setOption("abort-after", 5);
    //context.setOption("order-by", "name");
    context.applyCommandLine(argc, argv);

    //context.setOption("no-breaks", true);

    int res = context.run();

    //if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
    //    return res;          // propagate the result of the tests

    //int client_stuff_return_code = 0;
    //// your program - if the testing framework is integrated in your production code

    //return res + client_stuff_return_code; // the result from doctest is propagated here as well

    getchar();
    //system("pause");

    return 0;
}

TEST_CASE("boost shared_ptr weak_ptr") {
    ObjectPtr obj(new Object(1));


    std::cout << "ref count is " << obj.use_count() << std::endl;

    CHECK(obj.use_count() == 1);

    WeakObjectPtr weakObj(obj);
    WeakObjectPtr weakObj2(obj);

    CHECK(obj.use_count() == 1);

    // 弱指针提升引用计数 +1
    ObjectPtr p = weakObj.lock();
    if (p) {
        std::cout << p.unique() << std::endl;
    }

    CHECK(obj.use_count() == 2);

    print(obj);

    CHECK(obj.use_count() == 2);

    printRef(obj);

    CHECK(obj.use_count() == 2);

    ObjectPtr obj2(obj);

    CHECK(obj.use_count() == 3);

    // reset 引用计数 -1
    obj2.reset();

    CHECK(obj.use_count() == 2);
}


class parent;
class children;
class parent2;
class children2;

typedef boost::shared_ptr<parent> parent_ptr;
typedef boost::shared_ptr<children> children_ptr;

typedef boost::shared_ptr<parent2> parent2_ptr;
typedef boost::shared_ptr<children2> children2_ptr;

class parent {
public:
    ~parent() {
        std::cout << "destroying parent\n";
    }

public:
    children_ptr children;
};

class children {
public:
    ~children() {
        std::cout << "destroying children\n";
    }

public:
    parent_ptr parent;
};

class parent2 {
public:
    ~parent2() {
        std::cout << "destroying parent2\n";
    }

public:
    children2_ptr children;
};

class children2 {
public:
    ~children2() {
        std::cout << "destroying children2\n";
    }

public:
    boost::weak_ptr<parent2> parent;
};

void test() {
    parent_ptr father(new parent());
    children_ptr son(new children);

    father->children = son;
    son->parent = father;


}

void test2() {
    parent2_ptr father(new parent2());
    children2_ptr son(new children2);

    father->children = son;
    son->parent = father;
}



TEST_CASE("循环引用不释放") {

    std::cout << "begin test...\n";
    test();
    std::cout << "end test.\n";

}

TEST_CASE("循环弱引用释放") {

    std::cout << "begin test...\n";
    test2();
    std::cout << "end test.\n";

}