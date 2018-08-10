#include <iostream>
#include <samlib/ports.hpp>
#include <samlib/mailbox.hpp>

int main()
{
    using namespace std;

    samlib::ports<samlib::mailbox<int>*,samlib::mailbox<float>*> test;

    auto m1 = samlib::mailbox<int>();
    auto m2 = samlib::mailbox<float>();

    test.get<0>() = &m1;
    test.get<1>() = &m2;

    cout << test.size() << endl;
    cout << test.send_to<1>(45) << endl;
    cout << "m1 size: " << m1.size_approx() << endl;
    cout << "m2 size: " << m2.size_approx() << endl;

    auto test2 = samlib::ports(&m2,&m1);
    cout << test2.size() << endl;
    cout << test2.send_to<1>(23) << endl;
    cout << "m1 size: " << m1.size_approx() << endl;
    cout << "m2 size: " << m2.size_approx() << endl;

}
