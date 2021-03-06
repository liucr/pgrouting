/*PGR-GNU*****************************************************************
File: floydWarshall.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer: 
Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: vicky_vergara@hotmail.com

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

#include "postgres.h"
#include "fmgr.h"
#include "funcapi.h"
#if PGSQL_VERSION > 92
#include "access/htup_details.h"
#endif

// #define DEBUG

#include "./../../common/src/debug_macro.h"
#include "./../../common/src/time_msg.h"
#include "./../../common/src/pgr_types.h"
#include "./../../common/src/postgres_connection.h"
#include "./../../common/src/edges_input.h"
#include "./floydWarshall_driver.h"

PGDLLEXPORT Datum floydWarshall(PG_FUNCTION_ARGS);

static
void
process(
        char* edges_sql,
        bool directed,
        Matrix_cell_t **result_tuples,
        size_t *result_count) {
    pgr_SPI_connect();

    PGR_DBG("Load data");
    pgr_edge_t *edges = NULL;
    size_t total_tuples = 0;
    pgr_get_edges_no_id(edges_sql, &edges, &total_tuples);

    if (total_tuples == 0) {
        PGR_DBG("No edges found");
        (*result_count) = 0;
        (*result_tuples) = NULL;
        pgr_SPI_finish();
        return;
    }
    PGR_DBG("Total %ld tuples in query:", total_tuples);

    clock_t start_t = clock();
    PGR_DBG("Starting processing");
    char *err_msg = NULL;
    char *log_msg = NULL;
    do_pgr_floydWarshall(
            edges,
            total_tuples,
            directed,
            result_tuples,
            result_count,
            &log_msg,
            &err_msg);
    time_msg(" processing FloydWarshall", start_t, clock());

    PGR_DBG("Returning %ld tuples\n", *result_count);

    if (log_msg) {
        PGR_DBG("LOG = %s\n", log_msg);
        free(log_msg);
    }

    if (err_msg) {
       if (*result_tuples) free(*result_tuples);
       elog(ERROR, " %s\n", err_msg);
    }

    pfree(edges);
    pgr_SPI_finish();
}


PG_FUNCTION_INFO_V1(floydWarshall);
PGDLLEXPORT Datum
floydWarshall(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
    uint32_t              call_cntr;
    uint32_t               max_calls;
    TupleDesc            tuple_desc;

    /**************************************************************************/
    /*                                                                        */
    Matrix_cell_t  *result_tuples = 0;
    size_t result_count = 0;
    /*                                                                        */
    /**************************************************************************/

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext   oldcontext;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);


        /*********************************************************************/
        /*                                                                   */
        // QUERY
        // CREATE OR REPLACE FUNCTION pgr_floydWarshalll(
        // edges_sql TEXT,
        // directed BOOLEAN,
        // OUT seq INTEGER,
        // OUT from_vid bigint,
        // OUT to_vid bigint,
        // OUT cost float)


        PGR_DBG("Calling process");
        process(
                pgr_text2char(PG_GETARG_TEXT_P(0)),
                PG_GETARG_BOOL(1),
                &result_tuples,
                &result_count);

        /*                                                                   */
        /*********************************************************************/

        funcctx->max_calls = (uint32_t)result_count;
        funcctx->user_fctx = result_tuples;
        if (get_call_result_type(fcinfo, NULL, &tuple_desc) != TYPEFUNC_COMPOSITE)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                         "that cannot accept type record")));

        funcctx->tuple_desc = tuple_desc;
        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();
    call_cntr = (uint32_t)funcctx->call_cntr;
    max_calls = (uint32_t)funcctx->max_calls;
    tuple_desc = funcctx->tuple_desc;
    result_tuples = (Matrix_cell_t*) funcctx->user_fctx;

    if (call_cntr < max_calls) {
        HeapTuple    tuple;
        Datum        result;
        Datum        *values;
        bool*        nulls;

        /*********************************************************************/
        values = palloc(3 * sizeof(Datum));
        nulls = palloc(3 * sizeof(bool));

        // postgres starts counting from 1
        values[0] = Int64GetDatum(result_tuples[call_cntr].from_vid);
        nulls[0] = false;
        values[1] = Int64GetDatum(result_tuples[call_cntr].to_vid);
        nulls[1] = false;
        values[2] = Float8GetDatum(result_tuples[call_cntr].cost);
        nulls[2] = false;
        /*********************************************************************/

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        // cleanup
        if (result_tuples) free(result_tuples);

        SRF_RETURN_DONE(funcctx);
    }
}

