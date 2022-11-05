#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include <time.h>

#include "onem2m.h"

int display(char* database)
{
    DB* dbp;
    DBC* dbcp;
    DBT key, data;
    int close_db, close_dbc, ret;

    close_db = close_dbc = 0;

    /* Open the database. */
    if ((ret = db_create(&dbp, NULL, 0)) != 0) {
        fprintf(stderr,
            "%s: db_create: %s\n", database, db_strerror(ret));
        return (1);
    }
    close_db = 1;

    /* Turn on additional error output. */
    dbp->set_errfile(dbp, stderr);
    dbp->set_errpfx(dbp, database);

    /* Open the database. */
    if ((ret = dbp->open(dbp, NULL, database, NULL,
        DB_UNKNOWN, DB_RDONLY, 0)) != 0) {
        dbp->err(dbp, ret, "%s: DB->open", database);
        goto err;
    }

    /* Acquire a cursor for the database. */
    if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        goto err;
    }
    close_dbc = 1;

    /* Initialize the key/data return pair. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    /* Walk through the database and print out the key/data pairs. */
    while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0) {
        if (strncmp(key.data, "ty", key.size) == 0 ||
            strncmp(key.data, "st", key.size) == 0 ||
            strncmp(key.data, "cni", key.size) == 0 ||
            strncmp(key.data, "cbs", key.size) == 0 ||
            strncmp(key.data, "cs", key.size) == 0
            ) {
            printf("%.*s : %d\n", (int)key.size, (char*)key.data, *(int*)data.data);
        }
        else if (strncmp(key.data, "rr", key.size) == 0) {
            printf("%.*s : ", (int)key.size, (char*)key.data);
            if (*(bool*)data.data == true)
                printf("true\n");
            else
                printf("false\n");
        }

        else {
            printf("%.*s : %.*s\n",
                (int)key.size, (char*)key.data,
                (int)data.size, (char*)data.data);
        }
    }
    if (ret != DB_NOTFOUND) {
        dbp->err(dbp, ret, "DBcursor->get");
        printf("Cursor ERROR\n");
        exit(0);
    }


err:    if (close_dbc && (ret = dbcp->close(dbcp)) != 0)
dbp->err(dbp, ret, "DBcursor->close");
if (close_db && (ret = dbp->close(dbp, 0)) != 0)
fprintf(stderr,
    "%s: DB->close: %s\n", database, db_strerror(ret));
return (0);
}

int Store_CSE(CSE *cse_object) {
    char* DATABASE = "CSE.db";

    DB* dbp;    // db handle
    DBC* dbcp;
    int ret;        // template value

    DBT key_ri;
    DBT data;  // storving key and real data


    // if input == NULL
    if (cse_object->ri == NULL) {
        fprintf(stderr, "ri is NULL\n");
        return 0;
    }
    
    if (cse_object->rn == NULL) cse_object->rn = "";
    if (cse_object->pi == NULL) cse_object->pi = "NULL";
    if (cse_object->ty == '\0') cse_object->ty = -1;
    if (cse_object->ct == NULL) cse_object->ct = "";
    if (cse_object->lt == NULL) cse_object->lt = "";
    if (cse_object->csi == NULL) cse_object->csi = "";
  
    ret = db_create(&dbp, NULL, 0);
    if (ret) {
        fprintf(stderr, "db_create : %s\n", db_strerror(ret));
        fprintf(stderr, "File ERROR\n");
        return 0;
    }

    /*Set duplicate*/
    /*
    ret = dbp->set_flags(dbp, DB_DUP);
    if (ret != 0) {
        dbp->err(dbp, ret, "Attempt to set DUPSORT flag failed.");
        fprintf(stderr, "Flag Set ERROR\n");
        dbp->close(dbp, 0);
        return(ret);
    }
    */

    /*DB Open*/
    ret = dbp->open(dbp, NULL, DATABASE, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret) {
        dbp->err(dbp, ret, "%s", DATABASE);
        fprintf(stderr, "DB Open ERROR\n");
        return 0;
    }

    /*
  * The DB handle for a Btree database supporting duplicate data
  * items is the argument; acquire a cursor for the database.
  */
    if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
        dbp->err(dbp, ret, "DB->cursor");
        fprintf(stderr, "Cursor ERROR");
        return 0;
    }

    /* keyand data must initialize */
    memset(&key_ri, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    /* initialize the data to be the first of two duplicate records. */
    key_ri.data = cse_object->ri;
    key_ri.size = strlen(cse_object->ri) + 1;

    /* List data excluding 'ri' as strings using delimiters. */
    char str[DB_STR_MAX]= "\0";
    sprintf(str, "%s,%s,%d,%s,%s,%s",
            cse_object->rn,cse_object->pi,cse_object->ty,cse_object->ct,cse_object->lt,cse_object->csi);

    data.data = str;
    data.size = strlen(str) + 1;

    /* input DB */
    if ((ret = dbcp->put(dbcp, &key_ri, &data, DB_KEYLAST)) != 0)
        dbp->err(dbp, ret, "db->cursor");

    /* DB close */
    dbcp->close(dbcp);
    dbp->close(dbp, 0); 

    return 1;
}

