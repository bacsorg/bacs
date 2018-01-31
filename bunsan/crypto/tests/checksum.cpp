#define BOOST_TEST_MODULE checksum
#include <boost/test/unit_test.hpp>

#include <bunsan/crypto/checksum.hpp>
#include <bunsan/test/filesystem/tempfile.hpp>
#include <bunsan/test/filesystem/write_data.hpp>

namespace checksum = bunsan::crypto::checksum;

BOOST_AUTO_TEST_SUITE(checksum_)

BOOST_AUTO_TEST_CASE(decode) {
  bunsan::test::filesystem::tempfile tmp;
  bunsan::test::filesystem::write_data(tmp.path, "");
  BOOST_CHECK_EQUAL(checksum::md5(tmp.path),
                    "D41D8CD98F00B204E9800998ECF8427E");
  BOOST_CHECK_EQUAL(checksum::sha256(tmp.path),
                    "E3B0C44298FC1C149AFBF4C8996FB924"
                    "27AE41E4649B934CA495991B7852B855");
  BOOST_CHECK_EQUAL(checksum::sha384(tmp.path),
                    "38B060A751AC96384CD9327EB1B1E36A"
                    "21FDB71114BE07434C0CC7BF63F6E1DA"
                    "274EDEBFE76F65FBD51AD2F14898B95B");
  BOOST_CHECK_EQUAL(checksum::sha512(tmp.path),
                    "CF83E1357EEFB8BDF1542850D66D8007"
                    "D620E4050B5715DC83F4A921D36CE9CE"
                    "47D0D13C5D85F2B0FF8318D2877EEC2F"
                    "63B931BD47417A81A538327AF927DA3E");

  bunsan::test::filesystem::write_data(tmp.path, "hello world");
  BOOST_CHECK_EQUAL(checksum::md5(tmp.path),
                    "5EB63BBBE01EEED093CB22BB8F5ACDC3");
  BOOST_CHECK_EQUAL(checksum::sha256(tmp.path),
                    "B94D27B9934D3E08A52E52D7DA7DABFA"
                    "C484EFE37A5380EE9088F7ACE2EFCDE9");
  BOOST_CHECK_EQUAL(checksum::sha384(tmp.path),
                    "FDBD8E75A67F29F701A4E040385E2E23"
                    "986303EA10239211AF907FCBB83578B3"
                    "E417CB71CE646EFD0819DD8C088DE1BD");
  BOOST_CHECK_EQUAL(checksum::sha512(tmp.path),
                    "309ECC489C12D6EB4CC40F50C902F2B4"
                    "D0ED77EE511A7C7A9BCD3CA86D4CD86F"
                    "989DD35BC5FF499670DA34255B45B0CF"
                    "D830E81F605DCF7DC5542E93AE9CD76F");
}

BOOST_AUTO_TEST_SUITE_END()  // checksum_
