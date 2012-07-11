/*
 * Copyright (C) 2012 Remo Hertig
 * https://launchpad.net/anarchNet
 *
 * This file is part of anarchNet.
 *
 * anarchNet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * anarchNet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with anarchNet.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/concept_check.hpp>
#include "singleton.h"
#include "lru_cache.h"
#include "anarchNet.h"
#include "logger.h"

#define FUSE_USE_VERSION 27
#include <fusekit/daemon.h>
#include <fusekit/default_directory.h>
#include <fusekit/stream_object_file.h>
#include <fusekit/new_creator.h>

#ifndef SRC_DAEMON_STORAGE_H_
#define SRC_DAEMON_STORAGE_H_


#define BLOCKSIZE ConfigManager::instance().blocksize()
#define ID_SIZE 512
namespace an {
  typedef std::string fid_t;
  
  class an_file;
  
  class block
  {
  public:
    block(char *p,uint s) : data(p),size(s) { }
    block(const char *p,uint s) : size(s) { data = new char[size]; memcpy(data,p,size); }
    ~block() { delete[] data; }
    fid_t hash();
    char * data;
    uint size;
  };
  typedef boost::shared_ptr<block> smart_block;
  
  class block_request {
  };  
  
  struct block_header
  {
    unsigned long size;
    char next[AN_ID_LENGTH];
  };
  
  class file {
  public:
    file() : size(0), read_offset(0), write_offset(0), _current_block_id(0) {}
    bool add_block(smart_block b);
    bool store();
    fid_t id;
    int size;
    long read_offset;
    long write_offset;
    int block_for_offset(long);
    bool write(const char *data, long size);
    std::vector<fid_t> block_ids;
    std::vector<smart_block> data_blocks;
    std::vector<smart_block> header_blocks;
    void reset_offset() { write_offset = 0; _block_offset = 0; _current_block_id=0; _current_block= data_blocks.front(); }
    void next_block() { _current_block = data_blocks[++_current_block_id]; _block_offset = 0; }
  private:
    long _block_offset;
    smart_block _current_block;
    int _current_block_id;
  };
  typedef boost::shared_ptr<file> smart_file;
  class file_request {
  };
  
  class StorageManager;
  smart_file get(std::string fn);
  struct an_file : public fusekit::iostream_object_file< an_file >::type{
    an_file(std::string fn) 
    : fusekit::iostream_object_file< an_file >::type(*this)
    , fn(fn), sf(get(fn)){
            LOG(DEBUG) << "open file:" << fn;
    }
    ~an_file() {
      sf->store();
      LOG(DEBUG) << "closed file:" << fn;
    }
    std::string fn;
    smart_file sf;
  };
  
  std::ostream& operator<<( std::ostream& os, const an_file& f );
  std::istream& operator>>( std::istream& is, an_file& f );
  
  
  struct an_directory 
  : public fusekit::default_directory<
  fusekit::directory_factory< fusekit::new_creator< an_directory > >,
  fusekit::file_factory< fusekit::new_creator< an_file > >
  >::type {
    an_directory(){
      
    }
  };
  
  
  class StorageManager : public Singleton<StorageManager> {
    friend class Singleton<StorageManager>;
  public:
    bool init(const std::string&);
    void run();
    smart_file open(fid_t id);
    smart_file load_file(fid_t id);
  private:
    fusekit::daemon<an_directory> *_daemon;
    typedef lru_cache_using_boost_set<std::string,an::smart_file>::type  file_cache_type;
    
    boost::shared_ptr<file_cache_type> _file_cache;
  };
}

#endif