#ifndef IPT_H
#define IPT_H

#include<iostream>
#include<functional>
#include"hasher.h"
#include<cmath>

#define int_8bit unsigned char
#define int_16bit unsigned short int
#define int_32bit unsigned int
#define int_64bit unsigned long int

class PageTable;

class Process
{
private:
	int_16bit PID;
	int_32bit size_B;
public:

	Process(int_16bit PID,int_32bit size_B):
		PID(PID),size_B(size_B){}

	inline int_16bit get_PID()const{return PID;}
	inline int_32bit get_size()const{return size_B;}
};


class Entry
{
	private:
		
		int_16bit PID;
		int_16bit page_id;
		//bit from right to left:
		//possess, dirty
		int_16bit state_bit;
	public:

		Entry(int_16bit PID=0,int_16bit page_id=0,int_16bit state_bit=0):
			PID(PID),page_id(page_id),state_bit(state_bit){}


		inline int_16bit get_PID()const {return PID;}
		inline int_16bit get_page_id()const {return page_id;}

		inline void add_process(int_16bit PID,int_16bit page_id)
		{
			this->PID = PID;
			this->page_id = page_id;
			
			set_possess();
			set_clean();
		}
		inline bool  if_possess() const
		{
			return (state_bit & 1) == 1;
		}
		inline void set_possess()
		{
			state_bit |= 1;
		}
		inline void set_free()
		{
			state_bit &= 510;
		}
		inline bool if_dirty() const
		{
			return (state_bit & 2) == 2;
		}
		inline void set_dirty() 
		{
			state_bit |= 2;
		}
		inline void set_clean()
		{
			state_bit &= 509;
		}
		void show()
		{
			std::cout<<"PID: "<<PID<<" page_id: "<<page_id<<" state: "<<state_bit<<std::endl;
		}

};


class PageTable
{
private:
	
	int_16bit menmory_size_MB;
	int_16bit page_size_KB;
		
	int_32bit page_count;
	Entry* entrys;
	Hasher* hasher;

	int_32bit max_usabile_address;
public:
	PageTable(int_16bit menmory_size_MB,int_16bit page_size_KB):
		menmory_size_MB(menmory_size_MB),
		page_size_KB(page_size_KB)
	{
		int page_tatol_size_B = page_size_KB*1024+sizeof(Entry);
		page_count = (menmory_size_MB*1024*1024)/page_tatol_size_B;
		entrys = new Entry[page_count];
		max_usabile_address = page_count*page_size_KB*1024 - 1;
    }

	void set_hasher(Hasher * h)
	{
		hasher = h;
		h->set_result_range(0,page_count-1);
	}
	
    bool add_process(const Process& p)
	{
		
        int_16bit page_count_of_process = p.get_size()/(page_size_KB*1024);
		if(p.get_size()%page_size_KB != 0)
            page_count_of_process++;

        for (int i=0;i<page_count_of_process;i++)
		{
			int_32bit address = hasher->hash(p.get_PID(),i);
			entrys[address].add_process(p.get_PID(),i);
		}
		return true;
    }

    int_32bit get_page_id (int_16bit PID,int_16bit page_number) const {
        return hasher->hash(PID,page_number);
    }

    inline int_32bit get_physical_address(int_16bit PID,int_16bit page,int_16bit address_in_page) const {
        return get_page_id(PID,page)*page_size_KB*1024+address_in_page;
    }

    void clean(){
        delete[] entrys;
        entrys = new Entry[page_count];
    }

    inline const Entry* get_entrys()const
	{
        return entrys;
	}

    void run_process(const  Process& p){
        int_16bit page_count_of_process = p.get_size()/(page_size_KB*1024);
        if(p.get_size()%page_size_KB != 0)
            page_count_of_process++;

        for (int i=0;i<page_count_of_process;i++)
        {
            int_32bit address = hasher->hash(p.get_PID(),i);
            entrys[address].add_process(p.get_PID(),i);
        }

    }

    inline int_32bit get_page_count()const {return page_count;}

    inline int_16bit get_page_size_KB()const {return page_size_KB;}

	~PageTable()
	{
		if(hasher) 
			delete hasher;
		delete[] entrys;
	}






    void some_way_to_show_the_current_state()const
    {
        for(int_32bit i=0;i<page_count;i++)
            if(entrys[i].if_possess()){
                std::cout<<"page number:"<<i<<" ";
                entrys[i].show();
            }
    }

	static void test()
	{
		PageTable pt(256,4);
		pt.set_hasher(new Hasher_1());
		std::cout<<"create pt 256 4 test self"<<std::endl;
		pt.test_self();
		std::cout<<"inited page status"<<std::endl;
		pt.some_way_to_show_the_current_state();
		std::cout<<"add one process"<<std::endl;
		Process p1(32225,34567);
		pt.add_process(p1);
		pt.some_way_to_show_the_current_state();
		std::cout<<"add more process"<<std::endl;
		pt.add_process(Process(32224,562998));
		pt.add_process(Process(224,5699));
		pt.add_process(Process(31224,12398));
		pt.some_way_to_show_the_current_state();
		std::cout<<"test finished"<<std::endl;

	}

	void test_self()
	{
		std::cout<<menmory_size_MB<<std::endl;
		std::cout<<page_size_KB<<std::endl;
		std::cout<<max_usabile_address<<std::endl;
		std::cout<<page_count<<std::endl;
    }

	void test_entry()
	{
		Entry e1;
		e1.show();

		std::cout<<e1.if_dirty()<<e1.if_possess()<<std::endl;
		e1.set_dirty();
		e1.set_possess();
		e1.show();
		std::cout<<e1.if_dirty()<<e1.if_possess()<<std::endl;
		e1.set_clean();
		e1.set_free();
		e1.show();
		std::cout<<e1.if_dirty()<<e1.if_possess()<<std::endl;
	}

};

struct entry_info
{
    int_32bit count;
    int_16bit PID;
    int_16bit page_id;
    bool if_possess;
    bool if_dirty;

    entry_info(){}

    entry_info(const Entry * entry,int_32bit count):
        count(count),
        PID(entry->get_PID()),
        page_id(entry->get_page_id()),
        if_possess(entry->if_possess()),
        if_dirty(entry->if_dirty()){}
};

class entrys_info
{
public:
    entry_info** entrys;
    int_32bit size;

    entrys_info(const PageTable* pt):
        entrys(new entry_info*[pt->get_page_count()]),size(0)
    {

        const Entry* entrys = pt->get_entrys();
        int_32bit count = pt->get_page_count();

        for(int_32bit i=0;i<count;i++)
        {
            if(entrys[i].if_possess())
                this->entrys[size++] = new entry_info(entrys+i,i);
        }
    }

    entrys_info(const PageTable* pt,const Process p)
        :size(0)
    {
        const Entry * entrys = pt->get_entrys();

        int_16bit page_size_KB = pt->get_page_size_KB();

        int_16bit page_count_of_process = p.get_size()/(page_size_KB*1024);
        if(p.get_size()%page_size_KB != 0)
            page_count_of_process++;

        size = page_count_of_process;

        this->entrys = new entry_info*[page_count_of_process];

        for(int i=0;i<page_count_of_process;i++){
            int_32bit page_address = pt->get_page_id(p.get_PID(),i);
            this->entrys[i] = new entry_info(entrys+page_address,page_address);
        }

    }

    ~entrys_info(){delete[] entrys;}

private:
    entrys_info(const entrys_info&)=delete;
    entrys_info& operator=(const entrys_info&)=delete;
};


#endif



