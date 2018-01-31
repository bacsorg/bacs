#pragma once

#include <curl/curl.h>

namespace bunsan {
namespace curl {

enum class proto : long {
  http = CURLPROTO_HTTP,
  https = CURLPROTO_HTTPS,
  ftp = CURLPROTO_FTP,
  ftps = CURLPROTO_FTPS,
  scp = CURLPROTO_SCP,
  sftp = CURLPROTO_SFTP,
  telnet = CURLPROTO_TELNET,
  ldap = CURLPROTO_LDAP,
  ldaps = CURLPROTO_LDAPS,
  dict = CURLPROTO_DICT,
  file = CURLPROTO_FILE,
  tftp = CURLPROTO_TFTP,
  imap = CURLPROTO_IMAP,
  imaps = CURLPROTO_IMAPS,
  pop3 = CURLPROTO_POP3,
  pop3s = CURLPROTO_POP3S,
  smtp = CURLPROTO_SMTP,
  smtps = CURLPROTO_SMTPS,
  rtsp = CURLPROTO_RTSP,
  rtmp = CURLPROTO_RTMP,
  rtmpt = CURLPROTO_RTMPT,
  rtmpe = CURLPROTO_RTMPE,
  rtmpte = CURLPROTO_RTMPTE,
  rtmps = CURLPROTO_RTMPS,
  rtmpts = CURLPROTO_RTMPTS,
  gopher = CURLPROTO_GOPHER,
  all = CURLPROTO_ALL
};

}  // namespace curl
}  // namespace bunsan
