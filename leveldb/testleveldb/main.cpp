#include <raft_server/leveldb/db.h>
#include <raft_server/leveldb/write_batch.h>




namespace
{
    leveldb::DB* db;
    leveldb::Options options;
    leveldb::Status status;
    leveldb::WriteBatch batch;
}

int main()
{

    options.create_if_missing = true;
    status  =  leveldb::DB::Open(options, "./adb", &db);
    if(!status.ok())
    {
        printf("error0000:%s\n", status.ToString().c_str());
    }


    //TODO 1 asynchronous
/*
    std::string key = "dan" ; 
    std::string value, value1;

    status = db->Get(leveldb::ReadOptions(), key, &value);
    if(!status.ok())
    {
        if(!status.IsNotFound())
        {
            printf("error0001:%s\n", status.ToString().c_str());
            goto hereclose;
        }
        else
        {
            value = "26";
        }
    }


    status = db->Put(leveldb::WriteOptions(), key, value);
    
    if(!status.ok())
    {
        printf("error0002:%s\n", status.ToString().c_str());
        goto hereclose;
    }
    else db->Get(leveldb::ReadOptions(), key, &value1);
    {
        printf("key:%s value:%s\n",key.c_str(), value1.c_str());
    }

    status = db->Delete(leveldb::WriteOptions(), key);

    if(!status.ok())
    {
        printf("error0003:%s\n", status.ToString().c_str());
        goto hereclose;
    }
*/

/*
    //TODO 2 asynchronous batch

    std::string key1 = "lulu";
    std::string key2 = "dan";
    std::string value1; 
    std::string value2;

    status = db->Get(leveldb::ReadOptions(), key2, &value1);
    if(!status.ok())
    {
        if(!status.IsNotFound())
        {
            printf("error0001:%s\n", status.ToString().c_str());
            goto hereclose;
        }
    }
    else
    {
        printf("key:%s value:%s\n", key2.c_str(), value1.c_str()); 
    }



    status = db->Put(leveldb::WriteOptions(), key1, "27");
    
    if(!status.ok())
    {
        printf("error0002:%s\n", status.ToString().c_str());
        goto hereclose;
    }

    status = db->Get(leveldb::ReadOptions(), key1, &value2);

    batch.Delete(key1);
    batch.Put(key2, value2);        //make dan 27
    status = db->Write(leveldb::WriteOptions(), &batch);
        
    if(!status.ok())
    {
        printf("error0003:%s\n", status.ToString().c_str());
        goto hereclose;
    }
*/ 
/*
    //TODO 3 sync
    
    leveldb::WriteOptions write_options;
    write_options.sync = true;
    status = db->Put(leveldb::WriteOptions(), "em", "27");
    
    if(!status.ok())
    {
        printf("error0003:%s\n", status.ToString().c_str());
        goto hereclose;
    }
*/

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for(it->SeekToFirst(); it->Valid(); it->Next())
    {
        printf("%s:%s\n", it->key().ToString().c_str(), it->value().ToString().c_str());
    }

    if(!it->status().ok())
    {
        printf("error0003:%s\n", it->status().ToString().c_str());
        goto hereclose;
    }


hereclose:
    printf("done!  closing db\n");
    delete it;
    delete db;      // 关闭db
}

