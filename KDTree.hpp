#pragma once

/*
 * file: KDTree.hpp
 * author: J. Frederico Carvalho
 *
 * This is an adaptation of the KD-tree implementation in rosetta code
 *  https://rosettacode.org/wiki/K-d_tree
 * It is a reimplementation of the C code using C++.
 * It also includes a few more queries than the original
 *
 */

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

using point_t = std::vector< double >;
using indexArr = std::vector< size_t >;
using pointIndex = typename std::pair< std::vector< double >, size_t >;

class KDDNode {
  public:
    using KDDNodePtr = std::shared_ptr< KDDNode >;
    size_t index;
    point_t x;
    KDDNodePtr left;
    KDDNodePtr right;

    // initializer
    KDDNode();
    KDDNode(const point_t &, const size_t &, const KDDNodePtr &,
           const KDDNodePtr &);
    KDDNode(const pointIndex &, const KDDNodePtr &, const KDDNodePtr &);
    ~KDDNode();

    // getter
    double coord(const size_t &);

    // conversions
    explicit operator bool();
    explicit operator point_t();
    explicit operator size_t();
    explicit operator pointIndex();
};

using KDDNodePtr = std::shared_ptr< KDDNode >;

KDDNodePtr NewKDDNodePtr();

// square euclidean distance
inline double dist2(const point_t &, const point_t &);
inline double dist2(const KDDNodePtr &, const KDDNodePtr &);

// euclidean distance
inline double dist(const point_t &, const point_t &);
inline double dist(const KDDNodePtr &, const KDDNodePtr &);

// Need for sorting
class comparer {
  public:
    size_t idx;
    explicit comparer(size_t idx_);
    inline bool compare_idx(
        const std::pair< std::vector< double >, size_t > &,  //
        const std::pair< std::vector< double >, size_t > &   //
    );
};

using pointIndexArr = typename std::vector< pointIndex >;

inline void sort_on_idx(const pointIndexArr::iterator &,  //
                        const pointIndexArr::iterator &,  //
                        size_t idx);

using pointVec = std::vector< point_t >;

class KDTree {
    KDDNodePtr root;
    KDDNodePtr leaf;

    KDDNodePtr make_tree(const pointIndexArr::iterator &begin,  //
                        const pointIndexArr::iterator &end,    //
                        const size_t &length,                  //
                        const size_t &level                    //
                       );

  public:
    KDTree() = default;
    explicit KDTree(pointVec point_array);

  private:
    KDDNodePtr nearest_(           //
        const KDDNodePtr &branch,  //
        const point_t &pt,        //
        const size_t &level,      //
        const KDDNodePtr &best,    //
        const double &best_dist   //
    );

    // default caller
    KDDNodePtr nearest_(const point_t &pt);

  public:
    point_t nearest_point(const point_t &pt);
    size_t nearest_index(const point_t &pt);
    pointIndex nearest_pointIndex(const point_t &pt);

  private:
    pointIndexArr neighborhood_(  //
        const KDDNodePtr &branch,  //
        const point_t &pt,        //
        const double &rad,        //
        const size_t &level       //
    );

  public:
    pointIndexArr neighborhood(  //
        const point_t &pt,       //
        const double &rad);

    pointVec neighborhood_points(  //
        const point_t &pt,         //
        const double &rad);

    indexArr neighborhood_indices(  //
        const point_t &pt,          //
        const double &rad);
};
