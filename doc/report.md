# 操作系统课程设计实验报告<br><br><br><br><br><br><br>题目九<br><br><br>基于杂凑技术的反置页表方法的<br>页式内存管理的模拟实现























 



姓名: <u>梁镜湖</u>

班级:<u>软件一班</u>

学号:<u>55170130</u>




## 设计要求

### 功能要求
(1)设计非虚拟的反置页表页式存储管理模块；

(2)内存空间及其划分（界面）

内存物理空间大小可选择：256M bytes，512M bytes；

每个页框的大小可选择：1K bytes，2K bytes，4K bytes；

(3)根据所选择的内存空间及其划分，显示对应的反置页表表项的项数及其所占空间的大小；

(4)随机产生多个进程的（进程号，进程逻辑空间大小）二元组并显示；

注：产生的所有进程逻辑页面数之和≤内存物理空间的总页框数；

每个进程逻辑空间大小≥4页；

产生的进程（二元组）个数≥4；

进程逻辑页面数= 进程逻辑空间大小÷页框大小

(5)实现针对上述情况，显示相应反置页表的相关表项内容：

​    反置页表表项序号、进程号、逻辑页号、冲突标志、空闲/占用标志

(6)随机选定上述产生的一个进程，并随机产生该进程的一个逻辑地址L：

L的正确性检查：L对应的逻辑页号应与该进程的逻辑空间大小相符合；

显示L的逻辑页号、页内偏移；

显示L对应的页框号；

显示L访问的实际物理地址 

### 实现
(1)反置页表表项个数= 内存大小÷页大小 ；

​     反置页表所占空间大小=表项个数×每个表项所占字节数；

(2)自定义杂凑函数Hash(进程号，逻辑页号)及冲突策略，确定反置页表表项位置；

(3)杂凑函数定义及冲突策略：Hash(pid,p)=pid*页框大小+p

其中：pid为产生的进程号；p为逻辑页号；

冲突策略可采用顺序探查法；

(4)页表表项＝（进程号，逻辑页号，冲突计数，空闲/占用标志）

注：实现时，由于页表项数较多，可把Hash函数确定的表项序号加到页表表项中，不显示空白表项；但计算反置页表所占空间时不应该加表项序号；

(5)对随机产生的进程个数的二元组，应根据“注”的要求进行正确性验证；

(6)进程逻辑地址为16位；地址信息均以十六进制的形式显示





## 设计分析

### 反置页表简介

一般意义上，每个进程都有一个相关页表。该进程所使用的每个页都在页表中有一项。这种页的表示方式比较自然，这是因为进程是通过页的虚拟地址来引用页的。操作系统必须将这种引用转换成物理内存地址。这种方法的缺点之一是每个页表可能有很多项。这些表可能消耗大量物理内存，却仅用来跟踪物理内存是如何使用的。

为了解决这个问题，可以使用**反向页表**（invertedpagetable）。反向页表对于每个真正的内存页或帧才有一个条目。每个条目包含保存在真正内存位置的页的虚拟地址以及拥有该页的进程的信息。因此，整个系统只有一个页表，对每个物理内存的页只有一条相应的条目。因为系统只有一个页表，而有多个地址空间映射物理内存，所以反向页表的条目中通常需要一个地址空间标识符，以确保一个特定进程的一个逻辑页可以映射到相应的物理帧。

**基于杂凑技术的反置页表方法**是通过每个进程的PID和进程内的页号来寻找内存中的页框,相比与一般的反置页表,进程无需维护一个自己的页表,只需要通过进程内部地址就可以通过哈希方法找出物理地址,优点是减少了内存的访问(无需访问进程的页表),缺点是增加了寻址的复杂度(哈希方法性能较差)。



##  实现设计

### 思路

1. 首先根据题目要求，计算各种变量（地址，ID等）长度，为进程，表项等物件设计合理的类

2. 根据根据上述类型的特点,设计管理杂凑方法的类,设计一个hasher接口(抽象类),通过实现接口的具体类来完成杂凑方法
3. 设计一个使用hasher管理数据(页表信息和表项)的页表类,实现内存管理等主要功能
4. 使用Qt库,提供UI



### 环境

平台：Linux

语言：c++ (GCC 8.3.0)

依赖：Qt



## 核心代码

src in github:[<https://github.com/yukiikorei/ipt_ui>]

### ipt.h

#### include以及重定义
```C++
 #ifndef IPT_H
#define IPT_H

#include<iostream>
#include<QDebug>
#include<functional>
#include"hasher.h"
#include<cmath>

#define int_8bit unsigned char
#define int_16bit unsigned short int
#define int_32bit unsigned int
#define int_64bit unsigned long int

```

#### Process类

```C++
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

```
#### Entry类

```C++
class Entry
{
	private:
		
		int_16bit PID;
        int_32bit page_id;
		//bit from right to left:
		//possess, dirty
        int_16bit state_bit;
        int_32bit conflict;

	public:

        Entry(int_16bit PID=0,int_32bit page_id=0,int_16bit state_bit=0):
            PID(PID),
            page_id(page_id),
            state_bit(state_bit),
            conflict(0){}


		inline int_16bit get_PID()const {return PID;}
        inline int_32bit get_page_id()const {return page_id;}

        inline void add_process(int_16bit PID,int_32bit page_id)
        {

            if(if_possess()&&!((this->PID)==PID&&(this->page_id)==page_id)){
                conflict++;
            }

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
        inline int_32bit get_conflict_count()const{
            return conflict;
        }
        void show()
		{
			std::cout<<"PID: "<<PID<<" page_id: "<<page_id<<" state: "<<state_bit<<std::endl;
		}

};
```

#### PageTable类
```C++
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
        page_count = ((int_32bit)menmory_size_MB*1024*1024)/page_tatol_size_B;
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
		
        int_32bit page_count_of_process = p.get_size()/(page_size_KB*1024);
		if(p.get_size()%page_size_KB != 0)
            page_count_of_process++;

        for (int_32bit i=0;i<page_count_of_process;i++)
		{
			int_32bit address = hasher->hash(p.get_PID(),i);
			entrys[address].add_process(p.get_PID(),i);
		}
		return true;
    }

    int_32bit get_page_id (int_16bit PID,int_32bit page_number) const {
        return hasher->hash(PID,page_number);
    }

    inline int_32bit get_physical_address(int_16bit PID,int_32bit page,int_16bit address_in_page) const {
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

        int_32bit page_count_of_process = p.get_size()/(page_size_KB*1024);
        if(p.get_size()%page_size_KB != 0)
            page_count_of_process++;

        for (int_32bit i=0;i<page_count_of_process;i++)
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

};
```

### hasher.h

#### hasher接口

```C++
class Hasher
{
public:
	
    virtual int_32bit hash(int_16bit PID,int_32bit page)const = 0;

	virtual void set_result_range(int_32bit result_range_begin,int_32bit result_range_end)=0;

	virtual ~Hasher(){}

};
```

#### hasher一个具体实现
```C++
class Hasher_1 : public Hasher
{
private:
	int_32bit result_range_begin;
	int_32bit result_range_end;

	int_32bit range;
public:

	void set_result_range(int_32bit result_range_begin,int_32bit result_range_end)
	{
		this->result_range_begin=result_range_begin;
		this->result_range_end=result_range_end;
		range = result_range_end-result_range_begin;
	}

    int_32bit hash(int_16bit PID,int_32bit page)const
	{
        int_32bit result = ((page*12357)<<16)+PID*53457;

		result%=range;
		result+= result_range_begin;
		return result;
	}
```

## 设计演试

### 启动-创建页表

![2019-10-14 15-00-33 的屏幕截图](/home/korei/Pictures/2019-10-14 15-00-33 的屏幕截图.png)

### 添加随机进程

![2019-10-14 15-56-21 的屏幕截图](/home/korei/Pictures/2019-10-14 15-56-21 的屏幕截图.png)

### 查看和运行某一进程

![2019-10-14 15-57-48 的屏幕截图](/home/korei/Pictures/2019-10-14 15-57-48 的屏幕截图.png)

![2019-10-14 15-58-04 的屏幕截图](/home/korei/Pictures/2019-10-14 15-58-04 的屏幕截图.png)

### 寻址

![2019-10-14 15-58-48 的屏幕截图](/home/korei/Pictures/2019-10-14 15-58-48 的屏幕截图.png)



## 评价总结

在本设计中，较好地反映了基于杂凑技术的反置页表实现，在程序设计上，提供了良好的接口，页表基本实现了反置页表的功能是运作方式。通过本设计，了解到了在操作系统中的内存管理相关知识，学习到在维护内存时的各个步骤和技术难点，通过编程模拟实现也使得知识得到了升华。