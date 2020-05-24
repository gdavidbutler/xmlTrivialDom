/*
 * XQL - XML in SQLite
 * Copyright (C) 2020 G. David Butler <gdb@dbSystems.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "sqlite3.h"
#include "xml.h"
#include "xql.h"

int
xqlSchema(
  sqlite3 *d
){
  int rc;

  if ((rc = sqlite3_exec(d
  ,"SAVEPOINT \"xqlSchema\";"
# include "xql.str"
   "RELEASE \"xqlSchema\";"
  ,0,0,0)))
    sqlite3_exec(d, "ROLLBACK TO \"xqlSchema\";", 0,0,0);
  return (rc);
}

static sqlite3_int64
si(
  sqlite3 *d
 ,sqlite3_stmt *s
 ,sqlite3_stmt *i
 ,const xmlSt_t *t
){
  sqlite3_int64 o;

  sqlite3_bind_text(s, 1, (const char *)t->s, t->l, SQLITE_STATIC);
  if (sqlite3_step(s) == SQLITE_ROW)
    o = sqlite3_column_int64(s, 0);
  else
    o = 0;
  sqlite3_reset(s);
  if (!o) {
    sqlite3_bind_text(i, 1, (const char *)t->s, t->l, SQLITE_STATIC);
    if (sqlite3_step(i) == SQLITE_DONE)
      o = sqlite3_last_insert_rowid(d);
    sqlite3_reset(i);
  }
  return (o);
}

struct cx {
  sqlite3 *db;
  sqlite3_stmt *stB;
  sqlite3_stmt *stC;
  sqlite3_stmt *stR;
  sqlite3_stmt *stTs;
  sqlite3_stmt *stTi;
  sqlite3_stmt *stCs;
  sqlite3_stmt *stCi;
  sqlite3_stmt *stEi;
  sqlite3_stmt *stAi;
  sqlite3_int64 *pth;
  unsigned int pthM;
  unsigned int pthN;
  int wb;
};

static int
cb(
  xmlTp_t t
 ,unsigned int l
 ,const xmlSt_t *g
 ,const xmlSt_t *n
 ,const xmlSt_t *v
 ,void *x
#define X ((struct cx *)x)
){
  unsigned char *s1;
  void *tv;
  sqlite3_int64 o1;
  sqlite3_int64 o2;
  xmlSt_t s2;
  int rc;

  switch (t) {

  case xmlTp_Eb:
    rc = sqlite3_step(X->stB);
    sqlite3_reset(X->stB);
    if (rc != SQLITE_DONE)
      goto exit;
    if (!(o1 = si(X->db, X->stTs, X->stTi, g + l - 1)))
      goto exit;
    sqlite3_bind_int64(X->stEi, 1, *(X->pth + l - 1));
    sqlite3_bind_int(X->stEi, 2, 0);
    sqlite3_bind_int64(X->stEi, 3, o1);
    rc = sqlite3_step(X->stEi);
    sqlite3_reset(X->stEi);
    if (rc != SQLITE_DONE)
      goto exit;
    if (X->pthM == X->pthN) {
      if (!(tv = sqlite3_realloc(X->pth, (X->pthM + 1) * sizeof (*X->pth))))
        goto exit;
      X->pth = tv;
      ++X->pthM;
    }
    *(X->pth + X->pthN) = sqlite3_last_insert_rowid(X->db);
    ++X->pthN;
    sqlite3_step(X->stC);
    sqlite3_reset(X->stC);
    break;

  case xmlTp_Ea:
    rc = sqlite3_step(X->stB);
    sqlite3_reset(X->stB);
    if (rc != SQLITE_DONE)
      goto exit;
    if (v->l) {
      if ((s1 = sqlite3_malloc(v->l))) {
        if ((rc = xmlDecodeBody(s1, v->l, v->s, v->l)) > (int)v->l) {
          if ((tv = sqlite3_realloc(s1, rc)))
            rc = xmlDecodeBody((s1 = tv), rc, v->s, v->l);
          else
            rc = -1;
        }
      } else
        rc = -1;
      if (rc > 0) {
        s2.s = s1;
        s2.l = rc;
        o1 = si(X->db, X->stCs, X->stCi, &s2);
      } else
        o1 = si(X->db, X->stCs, X->stCi, v);
      sqlite3_free(s1);
    } else
      o1 = si(X->db, X->stCs, X->stCi, v);
    if (!o1)
      goto exit;
    if (!n)
      o2 = 0;
    else if (!(o2 = si(X->db, X->stTs, X->stTi, n)))
      goto exit;
    sqlite3_bind_int64(X->stAi, 1, *(X->pth + l));
    sqlite3_bind_int(X->stAi, 2, o1);
    if (o2)
      sqlite3_bind_int64(X->stAi, 3, o2);
    else
      sqlite3_bind_null(X->stAi, 3);
    rc = sqlite3_step(X->stAi);
    sqlite3_reset(X->stAi);
    if (rc != SQLITE_DONE)
      goto exit;
    sqlite3_step(X->stC);
    sqlite3_reset(X->stC);
    break;

  case xmlTp_Ec:
    if (!n && !X->wb)
      break;
    rc = sqlite3_step(X->stB);
    sqlite3_reset(X->stB);
    if (rc != SQLITE_DONE)
      goto exit;
    if (v->l) {
      if ((s1 = sqlite3_malloc(v->l))) {
        if ((rc = xmlDecodeBody(s1, v->l, v->s, v->l)) > (int)v->l) {
          if ((tv = sqlite3_realloc(s1, rc)))
            rc = xmlDecodeBody((s1 = tv), rc, v->s, v->l);
          else
            rc = -1;
        }
      } else
        rc = -1;
      if (rc > 0) {
        s2.s = s1;
        s2.l = rc;
        o1 = si(X->db, X->stCs, X->stCi, &s2);
      } else
        o1 = si(X->db, X->stCs, X->stCi, v);
      sqlite3_free(s1);
    } else
      o1 = si(X->db, X->stCs, X->stCi, v);
    if (!o1)
      goto exit;
    sqlite3_bind_int64(X->stEi, 1, *(X->pth + l));
    sqlite3_bind_int(X->stEi, 2, 1);
    sqlite3_bind_int64(X->stEi, 3, o1);
    rc = sqlite3_step(X->stEi);
    sqlite3_reset(X->stEi);
    if (rc != SQLITE_DONE)
      goto exit;
    sqlite3_step(X->stC);
    sqlite3_reset(X->stC);
    break;

  case xmlTp_Ee:
    if (X->pthN)
      --X->pthN;
    break;

  }
  return (0);
exit:
  sqlite3_step(X->stR);
  sqlite3_reset(X->stR);
  return (1);
}
#undef X

int
xml2xql(
  sqlite3 *d
 ,sqlite3_int64 o
 ,const unsigned char *s
 ,unsigned int l
 ,unsigned int m
 ,int w
){
  xmlSt_t *tg;
  struct cx cx;
  int rc;

  rc = -SQLITE_ERROR;
  if (!d || !s)
    return (rc);
  if (!l)
    return (SQLITE_OK);
  if (m) {
    if (!(tg = sqlite3_malloc(m * sizeof (*tg))))
      return (rc);
  } else
    tg = 0;
  cx.db = d;
  cx.wb = w;
  cx.stB = cx.stC = cx.stR = cx.stTs = cx.stTi = cx.stCs = cx.stCi = cx.stEi = cx.stAi = 0;
  if (!(cx.pth = sqlite3_malloc(sizeof (*cx.pth))))
    goto exit;
  *cx.pth = o;
  cx.pthM = cx.pthN = 1;
  if ((rc = -sqlite3_exec(d, "SAVEPOINT \"xml2xql\";", 0,0,0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"SAVEPOINT \"xml2xqlCb\";"
   ,-1, &cx.stB, 0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"RELEASE \"xml2xqlCb\";"
   ,-1, &cx.stC, 0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"ROLLBACK TO \"xml2xqlCb\";"
   ,-1, &cx.stR, 0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"SELECT \"i\" FROM \"XqlT\" WHERE \"v\"=?1"
   ,-1, &cx.stTs, 0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"INSERT INTO \"XqlT\"(\"v\")VALUES(?1)"
   ,-1, &cx.stTi, 0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"SELECT \"i\" FROM \"XqlC\" WHERE \"v\"=?1"
   ,-1, &cx.stCs, 0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"INSERT INTO \"XqlC\"(\"v\")VALUES(?1)"
   ,-1, &cx.stCi, 0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"INSERT INTO \"XqlE\"(\"e\",\"o\",\"t\",\"v\")VALUES(?1,IFNULL((SELECT MAX(\"o\")+1 FROM \"XqlE\" WHERE \"e\"=?1),0),?2,?3)"
   ,-1, &cx.stEi, 0)))
    goto exit;
  if ((rc = -sqlite3_prepare_v2(d
   ,"INSERT INTO \"XqlA\"(\"e\",\"o\",\"v\",\"n\")VALUES(?1,IFNULL((SELECT MAX(\"o\")+1 FROM \"XqlA\" WHERE \"e\"=?1),0),?2,?3)"
   ,-1, &cx.stAi, 0)))
    goto exit;
  rc = xmlParse(cb, m, tg, s, l, &cx);
exit:
  sqlite3_free(cx.pth);
  sqlite3_finalize(cx.stAi);
  sqlite3_finalize(cx.stEi);
  sqlite3_finalize(cx.stCi);
  sqlite3_finalize(cx.stCs);
  sqlite3_finalize(cx.stTi);
  sqlite3_finalize(cx.stTs);
  sqlite3_finalize(cx.stR);
  sqlite3_finalize(cx.stC);
  sqlite3_finalize(cx.stB);
  sqlite3_exec(d, "RELEASE \"xml2xql\";", 0,0,0);
  sqlite3_free(tg);
  return (rc);
}

char *
xql2xml(
  sqlite3 *d
 ,sqlite3_int64 o
 ,unsigned int *l
){
  char *xml;
  const unsigned char *s1;
  unsigned char *s2;
  sqlite3_stmt *stE;
  sqlite3_stmt *stA;
  sqlite3_stmt *stT;
  sqlite3_stmt *stC;
  sqlite3_str *rs;
  sqlite3_int64 *li;
  unsigned int liN;
  int rc;

  stE = stA = stT = stC = 0;
  li = 0;
  liN = 0;
  if (!(rs = sqlite3_str_new(0))
   || sqlite3_str_errcode(rs))
    goto exit;
  if ((rc = sqlite3_exec(d, "SAVEPOINT \"xql2xml\";", 0,0,0)))
    goto exit;
  if ((rc = sqlite3_prepare_v2(d
  ,"WITH"
   " \"w1\"(\"l\",\"e\",\"o\",\"i\",\"t\",\"v\")AS("
              "SELECT 0,\"e1\".\"e\",\"e1\".\"o\",\"e1\".\"i\",\"e1\".\"t\",\"e1\".\"v\""
             " FROM \"XqlE\" \"e1\""
             " WHERE \"e1\".\"e\"=?1"
   " UNION ALL SELECT \"w1\".\"l\"+1,\"e1\".\"e\",\"e1\".\"o\",\"e1\".\"i\",\"e1\".\"t\",\"e1\".\"v\""
             " FROM \"w1\""
                  ",\"XqlE\" \"e1\""
             " WHERE \"e1\".\"e\"=\"w1\".\"i\""
   " ORDER BY 1 DESC,2 ASC,3 ASC"
    ")"
   "SELECT \"l\",\"i\",\"t\",\"v\" FROM \"w1\""
   ,-1, &stE, 0)))
    goto exit;
  if ((rc = sqlite3_prepare_v2(d
   ,"SELECT \"v\",\"n\" FROM \"XqlA\" WHERE \"e\"=?1 ORDER BY \"o\" ASC"
   ,-1, &stA, 0)))
    goto exit;
  if ((rc = sqlite3_prepare_v2(d
   ,"SELECT \"v\" FROM \"XqlT\" WHERE \"i\"=?1"
   ,-1, &stT, 0)))
    goto exit;
  if ((rc = sqlite3_prepare_v2(d
   ,"SELECT \"v\" FROM \"XqlC\" WHERE \"i\"=?1"
   ,-1, &stC, 0)))
    goto exit;
  sqlite3_bind_int64(stE, 1, o);
  while (sqlite3_step(stE) == SQLITE_ROW) {
    void *tv;
    unsigned int i;
    int qm;

    i = sqlite3_column_int(stE, 0);
    while (i < liN) {
      --liN;
      sqlite3_bind_int64(stT, 1, *(li + liN));
      if (sqlite3_step(stT) == SQLITE_ROW
       && (s1 = sqlite3_column_text(stT, 0))
       && *s1 != '?'
       && *s1 != '!') {
        sqlite3_str_append(rs, "</", 2);
        sqlite3_str_append(rs, (const char *)s1, sqlite3_column_bytes(stT, 0));
        sqlite3_str_append(rs, ">", 1);
      }
      sqlite3_reset(stT);
    }
    if (sqlite3_column_int(stE, 2)) {
      sqlite3_bind_int64(stC, 1, sqlite3_column_int64(stE, 3));
      if (sqlite3_step(stC) == SQLITE_ROW
       && (s1 = sqlite3_column_text(stC, 0))
       && (i = sqlite3_column_bytes(stC, 0))) {
        if ((s2 = sqlite3_malloc(i))) {
          if ((rc = xmlEncodeString(s2, i, s1, i)) > (int)i) {
            if ((tv = sqlite3_realloc(s2, rc)))
              rc = xmlEncodeString((s2 = tv), rc, s1, i);
            else
              rc = -1;
          }
        } else
          rc = -1;
        if (rc > 0)
          sqlite3_str_append(rs, (const char *)s2, rc);
        else
          sqlite3_str_append(rs, (const char *)s1, i);
        sqlite3_free(s2);
      }
      sqlite3_reset(stC);
    } else {
      if (!(tv = sqlite3_realloc(li, (liN + 1) * sizeof (*li))))
        break;
      li = tv;
      *(li + liN) = sqlite3_column_int64(stE, 3);
      sqlite3_bind_int64(stT, 1, *(li + liN));
      ++liN;
      if (sqlite3_step(stT) == SQLITE_ROW
       && (s1 = sqlite3_column_text(stT, 0))) {
        sqlite3_str_append(rs, "<", 1);
        sqlite3_str_append(rs, (const char *)s1, sqlite3_column_bytes(stT, 0));
        if (*s1 == '?')
          qm = 1;
        else
          qm = 0;
      } else
        qm = 0;
      sqlite3_reset(stT);
      sqlite3_bind_int64(stA, 1, sqlite3_column_int64(stE, 1));
      while (sqlite3_step(stA) == SQLITE_ROW) {
        sqlite3_str_append(rs, " ", 1);
        if (sqlite3_column_type(stA, 1) != SQLITE_NULL) {
          sqlite3_bind_int64(stT, 1, sqlite3_column_int64(stA, 1));
          if (sqlite3_step(stT) == SQLITE_ROW) {
            s1 = sqlite3_column_text(stT, 0);
            sqlite3_str_append(rs, (const char *)s1, sqlite3_column_bytes(stT, 0));
            sqlite3_str_append(rs, "=", 1);
          }
          sqlite3_reset(stT);
        }
        sqlite3_str_append(rs, "\"", 1);
        sqlite3_bind_int64(stC, 1, sqlite3_column_int64(stA, 0));
        if (sqlite3_step(stC) == SQLITE_ROW
         && (s1 = sqlite3_column_text(stC, 0))
         && (i = sqlite3_column_bytes(stC, 0))) {
          if ((s2 = sqlite3_malloc(i))) {
            if ((rc = xmlEncodeString(s2, i, s1, i)) > (int)i) {
              if ((tv = sqlite3_realloc(s2, rc)))
                rc = xmlEncodeString((s2 = tv), rc, s1, i);
              else
                rc = -1;
            }
          } else
            rc = -1;
          if (rc > 0)
            sqlite3_str_append(rs, (const char *)s2, rc);
          else
            sqlite3_str_append(rs, (const char *)s1, i);
          sqlite3_free(s2);
        }
        sqlite3_reset(stC);
        sqlite3_str_append(rs, "\"", 1);
      }
      sqlite3_reset(stA);
      if (qm)
        sqlite3_str_append(rs, "?", 1);
      sqlite3_str_append(rs, ">", 1);
    }
  }
  while (liN) {
    --liN;
    sqlite3_bind_int64(stT, 1, *(li + liN));
    if (sqlite3_step(stT) == SQLITE_ROW
     && (s1 = sqlite3_column_text(stT, 0))
     && *s1 != '?'
     && *s1 != '!') {
      sqlite3_str_append(rs, "</", 2);
      sqlite3_str_append(rs, (const char *)s1, sqlite3_column_bytes(stT, 0));
      sqlite3_str_append(rs, ">", 1);
    }
    sqlite3_reset(stT);
  }
exit:
  sqlite3_free(li);
  sqlite3_finalize(stC);
  sqlite3_finalize(stT);
  sqlite3_finalize(stA);
  sqlite3_finalize(stE);
  sqlite3_exec(d, "RELEASE \"xql2xml\";", 0,0,0);
  if (!rs)
    xml = 0;
  else if (sqlite3_str_errcode(rs))
    xml = sqlite3_str_finish(rs);
  else if ((rc = sqlite3_str_length(rs)) > 0) {
    if ((xml = sqlite3_str_finish(rs)) && l)
      *l = rc;
  } else {
    (void)sqlite3_str_finish(rs);
    if ((xml = sqlite3_malloc(1))) {
      *xml = '\0';
      if (l)
        *l = 0;
    }
  }
  return (xml);
}
