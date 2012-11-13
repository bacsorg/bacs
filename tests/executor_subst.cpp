#include "bunsan/utility/executor.hpp"

#include <sstream>

#include <cassert>

#include <boost/property_tree/info_parser.hpp>

int main()
{
    using namespace boost::property_tree;
    using bunsan::utility::executor;
    typedef std::stringstream ss;
    typedef std::vector<std::string> vs;
    ptree pt;
    ss in_1(R"EOF(
t 1
t 2
t 3)EOF");
    read_info(in_1, pt);
    vs out_1 = {"1", "2", "3"};
    assert(executor(pt).context().built().argv()==out_1);
    ss in_2(R"EOF(
t 1
p 0
t 3
p 2
t 5
p 1)EOF");
    read_info(in_2, pt);
    vs out_2 = {"1", "2", "3", "4", "5", "6"};
    assert(executor(pt).add_argument("2", "6", "4").context().built().argv()==out_2);
    ss in_3(R"EOF(
t 1       ; 1
c         ; 234
{
    p 0   ; 2
    t 3   ; 3
    p 3   ; 4
}
p 2       ; 5
c
{
    p 1   ; 678
})EOF");
    read_info(in_3, pt);
    vs out_3 = {"1", "234", "5", "678"};
    assert(executor(pt).add_argument("2", "678", "5", "4").context().built().argv()==out_3);
    ss in_4(R"EOF(
n first       ; 1
c             ; _2_3_4
{
    n second  ; _2
    n third   ; _3
    n _4      ; _4
}
p 2           ; 5
n _5          ; _5
c             ; 678
{
    p 0       ; 6
    n _6      ; 7
    p 1       ; 8
})EOF");
    read_info(in_4, pt);
    vs out_4 = {"1", "_2_3_4", "5", "_5", "678"};
    assert(executor(pt)
            .add_argument("6", "8", "5")
            .set_argument("first", "1")
            .set_argument("second", "_2")
            .set_argument("third", "_3")
            .set_argument("_4", "_4")
            .set_argument("_5", "_5")
            .set_argument("_6", "7").context().built().argv()==out_4);
    ss in_5(R"EOF(
n first       ; 1
c             ; _2_3_4
{
    n second  ; _2
    n third   ; _3
    n _4      ; _4
}
p 2           ; 5
n _5          ; _5
c             ; 678
{
    p 0       ; 6
    n _6      ; 7
    p 1       ; 8
}
d
{
    first 1
    second _2
    third _3
    _4 _4
    _5 _5
    _6 7
})EOF");
    read_info(in_5, pt);
    vs out_5 = out_4;
    assert(executor(pt).add_argument("6", "8", "5").context().built().argv()==out_5);
    ss in_6(R"EOF(
t 1
t 2
p 1)EOF");
    read_info(in_6, pt);
    vs out_6 = {"1", "2", "3", "4"};
    assert(executor(pt).add_argument("O_O", "3", "4").context().built().argv()==out_6);
}
