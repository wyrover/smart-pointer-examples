#if _MSC_VER >= 1400
//  Following 8 lines: workaround for a bug in some older SDKs
#   pragma push_macro("_interlockedbittestandset")
#   pragma push_macro("_interlockedbittestandreset")
#   pragma push_macro("_interlockedbittestandset64")
#   pragma push_macro("_interlockedbittestandreset64")
#   define _interlockedbittestandset _local_interlockedbittestandset
#   define _interlockedbittestandreset _local_interlockedbittestandreset
#   define _interlockedbittestandset64 _local_interlockedbittestandset64
#   define _interlockedbittestandreset64 _local_interlockedbittestandreset64
#   include <intrin.h> // to force the header not to be included elsewhere
#   pragma pop_macro("_interlockedbittestandreset64")
#   pragma pop_macro("_interlockedbittestandset64")
#   pragma pop_macro("_interlockedbittestandreset")
#   pragma pop_macro("_interlockedbittestandset")
#endif


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
#include <boost/thread.hpp>


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


void reset(boost::shared_ptr<int>& sh) {
    sh.reset();
}

void print4(boost::weak_ptr<int>& w) {
    boost::shared_ptr<int> sh = w.lock();
    if (sh)
        std::cout << *sh << '\n';
}


TEST_CASE("提升 weak_ptr") {



}



class A {
    class deleter {
    public:
        void operator()(A* p) {
            delete p;
        }
    };
    friend class deleter;
public:



    virtual void sing() {
        std::cout << "Lalalalalalalalalalala";
    }

    static boost::shared_ptr<A> createA() {
        boost::shared_ptr<A> p(new A(), A::deleter());
        return p;
    }

protected:
    virtual ~A() {
        printf("delete object A.\n");
    };
};

TEST_CASE("定制删除器") {

    boost::shared_ptr<A> p = A::createA();

}


class Test {
public:
    //析构函数
    ~Test() {
        std::cout << "Test Destructor." << std::endl;
    }
    //获取指向当前对象的指针
    boost::shared_ptr<Test> GetObject() {
        boost::shared_ptr<Test> pTest(this);
        return pTest;
    }
};


TEST_CASE("返回 this 指针的智能指针错误示范") {

    // 析构两次，错误
    boost::shared_ptr<Test> p(new Test());
    boost::shared_ptr<Test> q = p->GetObject();

}


class Test2 : public boost::enable_shared_from_this<Test2> {      //改进1
public:
    //析构函数
    ~Test2() {
        std::cout << "Test2 Destructor." << std::endl;
    }
    //获取指向当前对象的指针
    boost::shared_ptr<Test2> GetObject() {
        return shared_from_this();      //改进2
    }
};


TEST_CASE("返回 this 指针的智能指针正确示范") {

    // 析构两次，错误
    boost::shared_ptr<Test2> p(new Test2());
    boost::shared_ptr<Test2> q = p->GetObject();

}


class Test3 : public boost::enable_shared_from_this<Test3>
{
public:
    void func() { boost::shared_ptr<Test3> pTest = shared_from_this(); }
};

TEST_CASE("实例化 boost::enable_shared_from_this 对象的错误示范") {

    //Test3 test;
    //test.func();    //错误
    //Test3* pTest = new Test3;
    //pTest->func(); //错误

}

TEST_CASE("实例化 boost::enable_shared_from_this 对象的正确示范") {

    boost::shared_ptr<Test3> pTest( new Test3() );
    pTest->func();

}