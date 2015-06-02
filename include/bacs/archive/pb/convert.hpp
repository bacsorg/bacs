#pragma once

#include <bacs/archive/pb/problem.pb.h>
#include <bacs/archive/problem.hpp>

#include <algorithm>

namespace bacs{namespace archive{namespace pb
{
    // we need class here to avoid forward-declarations
    struct detail
    {
        template <typename Entry, typename Sequence>
        static void convert(const Sequence &sequence, google::protobuf::RepeatedPtrField<Entry> &repeated)
        {
            repeated.Clear();
            repeated.Reserve(sequence.size());
            for (const auto &i: sequence)
                convert(i, *repeated.Add());
        }

        template <typename T>
        static void convert(const T &src, T &dst)
        {
            dst = src;
        }

        static void convert(const archive::problem::id_set &ids, problem::IdSet &proto)
        {
            convert(ids, *proto.mutable_ids());
        }

        static void convert(const archive::problem::info_map::value_type &id_info, problem::InfoMap::Entry &proto)
        {
            proto.set_id(id_info.first);
            if (id_info.second)
                convert(id_info.second.get(), *proto.mutable_info());
        }

        static void convert(const archive::problem::info_map &info_map, problem::InfoMap &proto)
        {
            convert(info_map, *proto.mutable_items());
        }

        static void convert(const archive::problem::hash_map::value_type &id_hash, problem::HashMap::Entry &proto)
        {
            proto.set_id(id_hash.first);
            if (id_hash.second)
                proto.set_hash(id_hash.second->data(), id_hash.second->size());
        }

        static void convert(const archive::problem::hash_map &hash_map, problem::HashMap &proto)
        {
            convert(hash_map, *proto.mutable_items());
        }

        static void convert(const archive::problem::flag_set &flags, problem::FlagSet &proto)
        {
            convert(flags, *proto.mutable_flags());
        }

        static void convert(const archive::problem::status &status, problem::Status &proto)
        {
            convert(status.flags, *proto.mutable_flags());
            proto.set_hash(status.hash.data(), status.hash.size());
        }

        static void convert(const archive::problem::status_map::value_type &id_status, problem::StatusMap::Entry &proto)
        {
            proto.set_id(id_status.first);
            if (id_status.second)
                convert(id_status.second.get(), *proto.mutable_status());
        }

        static void convert(const archive::problem::status_map &status_map, problem::StatusMap &proto)
        {
            convert(status_map, *proto.mutable_items());
        }

        static void convert(const archive::problem::import_info &import_info, problem::ImportInfo &proto)
        {
            if (import_info.error)
                proto.set_error(import_info.error.get());
            if (import_info.status)
                convert(import_info.status.get(), *proto.mutable_status());
        }

        static void convert(const archive::problem::import_map::value_type &id_import, problem::ImportMap::Entry &proto)
        {
            proto.set_id(id_import.first);
            convert(id_import.second, *proto.mutable_import_info());
        }

        static void convert(const archive::problem::import_map &import_map, problem::ImportMap &proto)
        {
            convert(import_map, *proto.mutable_items());
        }
    };

    template <typename Src, typename Dst>
    void convert(const Src &src, Dst &dst)
    {
        detail::convert(src, dst);
    }
}}}
