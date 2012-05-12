#include <sstream>

#include <cassert>

#include <boost/property_tree/info_parser.hpp>

#include "bunsan/utility/executor.hpp"

int main()
{
    using namespace boost::property_tree;
    using bunsan::utility::executor;
    typedef std::stringstream ss;
    typedef std::vector<std::string> vs;
    ptree pt;
    ss in_1(
        "t 1\n"
        "t 2\n"
        "t 3\n");
    read_info(in_1, pt);
    vs out_1 = {"1", "2", "3"};
    assert(executor(pt).context().built().argv()==out_1);
    ss in_2(
        "t 1\n"
        "p 0\n"
        "t 3\n"
        "p 2\n"
        "t 5\n"
        "p 1\n");
    read_info(in_2, pt);
    vs out_2 = {"1", "2", "3", "4", "5", "6"};
    assert(executor(pt).add_argument("2", "6", "4").context().built().argv()==out_2);
    ss in_3(
        "t 1\n"     //1
        "c\n"       //234
        "{\n"
        "\tp 0\n"   //2
        "\tt 3\n"   //3
        "\tp 3\n"   //4
        "}\n"
        "p 2\n"     //5
        "c\n"
        "{\n"
        "\tp 1\n"   //678
        "}\n");
    read_info(in_3, pt);
    vs out_3 = {"1", "234", "5", "678"};
    assert(executor(pt).add_argument("2", "678", "5", "4").context().built().argv()==out_3);
    ss in_4(
        "n first\n" //1
        "c\n"       //_2_3_4
        "{\n"
        "\tn second\n"  //_2
        "\tn third\n"   //_3
        "\tn _4\n"  //_4
        "}\n"
        "p 2\n"     //5
        "n _5\n"    //_5
        "c\n"       //678
        "{\n"
        "\tp 0\n"   //6
        "\tn _6\n"  //7
        "\tp 1\n"   //8
        "}\n");
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
    ss in_5(
        "n first\n" //1
        "c\n"       //_2_3_4
        "{\n"
        "\tn second\n"  //_2
        "\tn third\n"   //_3
        "\tn _4\n"  //_4
        "}\n"
        "p 2\n"     //5
        "n _5\n"    //_5
        "c\n"       //678
        "{\n"
        "\tp 0\n"   //6
        "\tn _6\n"  //7
        "\tp 1\n"   //8
        "}\n"
        "d\n"
        "{\n"
        "\tfirst 1\n"
        "\tsecond _2\n"
        "\tthird _3\n"
        "\t_4 _4\n"
        "\t_5 _5\n"
        "\t_6 7\n"
        "}\n");
    read_info(in_5, pt);
    vs out_5 = out_4;
    assert(executor(pt).add_argument("6", "8", "5").context().built().argv()==out_5);
    ss in_6(
        "t 1\n"
        "t 2\n"
        "p 1\n");
    read_info(in_6, pt);
    vs out_6 = {"1", "2", "3", "4"};
    assert(executor(pt).add_argument("O_O", "3", "4").context().built().argv()==out_6);
}

