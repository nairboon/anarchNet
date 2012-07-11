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

#include <cstdio>
#include <cstring>
#include <cmath>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include "storage.h"
#include "module_manager.h"
#include "config_manager.h"
#include "crypto.h"
#include <fusekit/daemon.h>
#include "logger.h"

namespace an {


 smart_file get(std::string fn)
 {
    return StorageManager::instance().open(fn);
 }
bool StorageManager::init(const std::string& mnt_point) {
	  _daemon = &fusekit::daemon<an_directory>::instance();
	    _file_cache = boost::shared_ptr<file_cache_type>(new file_cache_type(boost::bind(&StorageManager::load_file,this,_1),512));

    boost::shared_ptr<boost::thread> fuse = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&StorageManager::run,this)));
    LOG(INFO) << "starting storage system, blocksize: " << BLOCKSIZE;
    //_daemon->instance().root().mknod("test");
	    return true;
	}
void StorageManager::run() {
	   struct fuse_args args = FUSE_ARGS_INIT(0, NULL);

	  fuse_opt_add_arg(&args, "anDaemon");
         fuse_opt_add_arg(&args, "mnt");
	  fuse_opt_add_arg(&args, "-f");

     LOG(INFO) << args.argv[0];
     _daemon->run(args.argc,args.argv);
	}
	smart_file StorageManager::open(fid_t id)
	{
	 return (*_file_cache)(id);
	}
	
	     smart_file StorageManager::load_file(fid_t id)
	     {
	       smart_file res = smart_file(new file);
	       fid_t db_id = crypto::Hash("DataMap"+id);
	       an::KV_ResPtr kres;
	       if(ModuleManager::instance().kv_get(db_id,kres))
	       {
		 BOOST_FOREACH(const an::KV_ResMap::value_type& entry, *kres)
		 {
		   if(entry.second.size() == AN_ID_LENGTH+2) // +2 DH magic byte
		    if(entry.second[AN_ID_LENGTH] == 'D' && entry.second[AN_ID_LENGTH+1] == 'H') //its an old file
		    {
		      fid_t blockid = entry.second.substr(0,AN_ID_LENGTH);
		      smart_block header1;
		      if(!ModuleManager::instance().get_block(blockid,header1))
		      {
			LOG(ERROR) << "loading block:" << blockid;
			throw 1;
		      }
		      if(header1.get()->size < sizeof(block_header))
		      {
			LOG(ERROR) << "header too small:" << header1.get()->size;
			throw 1;
		      }
		      block_header raw_header;
		      memcpy(&raw_header,header1.get()->data,sizeof(block_header));
		      
		      int blocks_needed = ceil(raw_header.size / ConfigManager::instance().blocksize());
		      int blocks_per_file = ceil( (ConfigManager::instance().blocksize() - sizeof(block_header)) / AN_ID_LENGTH);
		      if(blocks_needed > blocks_per_file) 
		      {
			LOG(ERROR) << "needs multi headers: " << blocks_per_file << ":" << blocks_needed;
			throw 1;
		      }
		      res->size = raw_header.size;
		      int offset = sizeof(block_header);
		      for(int i = 0;i< blocks_needed;i++)
		      {
			char hash[AN_ID_LENGTH+1];
			memcpy(&hash,header1.get()->data+offset,AN_ID_LENGTH);
			hash[AN_ID_LENGTH] = '\0';
			res->block_ids.push_back(fid_t(hash));
			smart_block subblock;
			  if(!ModuleManager::instance().get_block(res->block_ids.back(),subblock))
			  {
			    LOG(ERROR) << "loading block:" << blockid;
			    throw 1;
			  }
			  res->data_blocks.push_back(subblock);
		      }
		    }
		 }
	       }
	      return res;
	     }
	     bool file::add_block(smart_block b)
	     {
	       data_blocks.push_back(b);
	       size+=b->size;
	       LOG(INFO) << "added block:" << b->size;
	       _current_block = b;
	       _current_block_id = data_blocks.size()-1;
	       return true;
	     }
	     bool file::store() // write new version of masterfile to disk
	     {
	     }
	     
	     std::ostream& operator<<( std::ostream& os, const an_file& f ){
	       
	      /* int b =f.sf->block_for_offset(f.sf->read_offset);
	       long relative_offset = f.sf->read_offset - (b*ConfigManager::instance().blocksize());
	       LOG(INFO) << "b: " << b << " ro: " << relative_offset;
	       for(int i = b;i<f.sf->data_blocks.size();i++)
	       {
		 os.write(b->data,b->size - relative_offset);
	       }
	       */
	      BOOST_FOREACH(smart_block b, f.sf->data_blocks)
	       os.write(b->data,b->size);
	       
	       LOG(INFO) << "read req: " << f.fn;
	     }
	     
	     std::istream& operator>>( std::istream& is, an_file& f ){
	       std::string buff;
	       long t_read = 0;
	       while(is.good()) {
		 
		 is >> buff;
		 int read = buff.size();
		 t_read += read;
		 LOG(DEBUG) << "read: " << read << " : " << t_read << " : " << buff;
		 
		 if(t_read >= BLOCKSIZE)
		 {
		   f.sf->write(buff.c_str(),t_read);
		   LOG(DEBUG) << "sent to file: " << t_read;
		   buff.clear();
		   t_read = 0;
		 }
	       }
	       // write remaining bytes
	       if(t_read > 0)
	       {
		  f.sf->write(buff.c_str(),t_read);
		   LOG(DEBUG) << "sent to file: " << t_read;
	       }
	       f.sf->reset_offset();
		 /*
		 LOG(INFO) << "write req: " << f.fn;
		 char *buff = new char[BLOCKSIZE+1];
		 is.read(buff,BLOCKSIZE);
		 int read = is.gcount();
		 // FIXME data > blocksize
		 if(read > 0)
		 {
		   LOG(DEBUG) << "wrote: " << buff << " :" << read;
		   if(f.sf->data_blocks.size() > 1)
		    memcpy(f.sf->data_blocks.front()->data,buff, BLOCKSIZE);
		     else
		   f.sf->add_block(smart_block(new block(buff,read)));
		 }
		 else
		   LOG(ERROR)<< "stream bad" << read;
	       }
	       else
		 LOG(ERROR)<< "stream bad" << f.fn;*/
	     
	     }
	     
	         int file::block_for_offset(long offset)
		 {
		   
		   return ceil(offset / ConfigManager::instance().blocksize());

		 }
		 
		 bool file::write(const char *data, long size)
		 {
		   LOG(DEBUG) << "file::write: " << size;
		   
		   if(size <= BLOCKSIZE)
		   {
		     if(data_blocks.size() == 0) // new file, no blocks
			    {
			      add_block(smart_block(new block(data,size)));
			    }
			    else
			    {
			      memcpy(_current_block->data,data,size);
			      _current_block->size = size;
			    }
		   }
		   else {
		     long rsize = BLOCKSIZE; 
		     int i = 0;
		     while(size > 0) {
		       if(size < BLOCKSIZE)
			 rsize = size;
		       
		       if(data_blocks.size() == 0) // new file, no blocks
			    {
			      add_block(smart_block(new block( data+i,rsize)));
			    }
			    else{
			      if(_block_offset > 0)
				LOG(ERROR) << "in-block write";
			      else
			       memcpy(_current_block->data+_block_offset,data,rsize);
			      _current_block->size = rsize;
			      next_block();
			    }
			    
			    i += rsize;
		       size -= rsize;  
		     }
		     _block_offset = rsize;
		   }
		   
		   
		 }
}
