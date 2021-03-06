/*PGR-GNU*****************************************************************
File: many_to_one_dijkstra_driver.cpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer: 
Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: vicky_vergara@hotmail.com

Copyright (c) 2015 pgRouting developers

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

********************************************************************PGR-GNU*/

#if defined(__MINGW32__) || defined(_MSC_VER)
#include <winsock2.h>
#include <windows.h>
#endif

#include <sstream>
#include <deque>
#include <vector>
#include <set>
#include "./pgr_dijkstra.hpp"
#include "./many_to_one_dijkstra_driver.h"

#include "../../common/src/pgr_alloc.hpp"
#include "./../../common/src/pgr_types.h"

// CREATE OR REPLACE FUNCTION pgr_dijkstra(
// sql text,
// start_vids anyarray,
// end_vid BIGINT,
// directed BOOLEAN default true,
void
do_pgr_many_to_one_dijkstra(
        pgr_edge_t  *data_edges,
        size_t total_tuples,
        int64_t  *start_vidsArr,
        size_t size_start_vidsArr,
        int64_t end_vid,
        bool directed,
        bool only_cost,
        General_path_element_t **return_tuples,
        size_t *return_count,
        char ** err_msg) {
    std::ostringstream log;
    try {
        graphType gType = directed? DIRECTED: UNDIRECTED;

        std::deque< Path >paths;
        log << "Inserting vertices into a c++ vector structure\n";
        std::set< int64_t > s_start_vertices(start_vidsArr, start_vidsArr + size_start_vidsArr);
        std::vector< int64_t > start_vertices(s_start_vertices.begin(), s_start_vertices.end());

        if (directed) {
            log << "Working with directed Graph\n";
            pgrouting::DirectedGraph digraph(gType);
            digraph.graph_insert_data(data_edges, total_tuples);
            pgr_dijkstra(digraph, paths, start_vertices, end_vid, only_cost);
        } else {
            log << "Working with Undirected Graph\n";
            pgrouting::UndirectedGraph undigraph(gType);
            undigraph.graph_insert_data(data_edges, total_tuples);
            pgr_dijkstra(undigraph, paths, start_vertices, end_vid, only_cost);
        }


        size_t count(0);
        count = count_tuples(paths);


        if (count == 0) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            log <<
                "No paths found between Starting and any of the Ending vertices\n";
            *err_msg = strdup(log.str().c_str());
            return;
        }

        (*return_tuples) = pgr_alloc(count, (*return_tuples));
        log << "Converting a set of paths into the tuples\n";
        (*return_count) = (collapse_paths(return_tuples, paths));


#ifndef DEBUG
        *err_msg = strdup("OK");
#else
        *err_msg = strdup(log.str().c_str());
#endif

        return;
    } catch ( ... ) {
        log << "Caught unknown exception!\n";
        *err_msg = strdup(log.str().c_str());
        return;
    }
}





