#pragma once

#include <bunsan/broker/task/channel.hpp>

#include <ostream>

namespace bunsan{namespace broker{namespace task
{
    class stream_channel: public channel
    {
    public:
        explicit stream_channel(std::ostream &out);

        void send_status(const Status &status) override;
        void send_result(const Result &result) override;

    private:
        std::ostream &m_out;
    };
}}}
