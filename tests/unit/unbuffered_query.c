/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012 Drizzle Developer Group
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 *     * The names of its contributors may not be used to endorse or
 * promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <yatl/lite.h>

#include <libdrizzle-redux/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_row_t row;
  int num_fields;

  drizzle_st *con = drizzle_create(getenv("MYSQL_SERVER"),
                                   getenv("MYSQL_PORT") ? atoi(getenv("MYSQL_PORT"))
                                                        : DRIZZLE_DEFAULT_TCP_PORT,
                                   getenv("MYSQL_USER"),
                                   getenv("MYSQL_PASSWORD"),
                                   getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  drizzle_return_t ret = drizzle_connect(con);
  if (ret == DRIZZLE_RETURN_COULD_NOT_CONNECT)
  {
    char error[DRIZZLE_MAX_ERROR_SIZE];
    strncpy(error, drizzle_error(con), DRIZZLE_MAX_ERROR_SIZE);
    drizzle_quit(con);
    SKIP_IF_(ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s(%s)", error,
             drizzle_strerror(ret));
  }
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_connect(): %s(%s)",
             drizzle_error(con), drizzle_strerror(ret));

  drizzle_query(con, "DROP SCHEMA IF EXISTS test_unbuff", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA test_unbuff (%s)",
             drizzle_error(con));

  drizzle_query(con, "CREATE SCHEMA test_unbuff", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA test_unbuff (%s)",
             drizzle_error(con));

  ret = drizzle_select_db(con, "test_unbuff");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "USE test_unbuff");

  drizzle_query(con, "CREATE TABLE test_unbuff.t1 (a INT)", 0, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Create table failure\n");
    return EXIT_FAILURE;
  }

  drizzle_query(con, "INSERT INTO test_unbuff.t1 VALUES (1),(2),(3)", 0, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Insert failure\n");
    return EXIT_FAILURE;
  }

  drizzle_result_st *result =
      drizzle_query(con, "SELECT * FROM test_unbuff.t1", 0, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Select failure\n");
    return EXIT_FAILURE;
  }

  if (drizzle_column_buffer(result) != DRIZZLE_RETURN_OK)
  {
    printf("Column buffer failure\n");
    return EXIT_FAILURE;
  }
  num_fields = drizzle_result_column_count(result);

  if (num_fields != 1)
  {
    printf("Retrieved bad number of fields\n");
    return EXIT_FAILURE;
  }

  int i = 0;
  char buf[10];
  while(1)
  {
    row = drizzle_row_buffer(result, &ret);
    if (ret != DRIZZLE_RETURN_OK)
    {
      printf("Row retrieval error\n");
      break;
    }
    if (row == NULL)
    {
      // EOF
      break;
    }
    i++;
    snprintf(buf, 10, "%d", i);
    if (strcmp(row[0], buf) != 0)
    {
      printf("Retrieved bad row data\n");
      return EXIT_FAILURE;
    }
    drizzle_row_free(result, row);
  }
  /* we should have had 3 rows */
  if (i != 3)
  {
    printf("Retrieved bad number of rows\n");
    return EXIT_FAILURE;
  }

  drizzle_result_free(result);

  drizzle_query(con, "DROP TABLE test_unbuff.t1", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP TABLE test_unbuff.t1");

  drizzle_query(con, "DROP SCHEMA IF EXISTS test_unbuff", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP SCHEMA test_unbuff (%s)",
             drizzle_error(con));

  ret = drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_strerror(ret));

  return EXIT_SUCCESS;
}
