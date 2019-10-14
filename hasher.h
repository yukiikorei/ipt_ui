#ifndef HASHER_H
#define HASHER_H

#define int_8bit unsigned char
#define int_16bit unsigned short int
#define int_32bit unsigned int
#define int_64bit unsigned long int

class Hasher
{
public:
	
    virtual int_32bit hash(int_16bit PID,int_32bit page)const = 0;

	virtual void set_result_range(int_32bit result_range_begin,int_32bit result_range_end)=0;

	virtual ~Hasher(){}

};


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

/*

	static void test()
	{
		std::cout<<"begin test hasher_1"<<std::endl;
		
		int turn = 1;
		Hasher_1 hasher;
		hasher.set_result_range(0,(int_32bit)4294967291);
		
_BEGIN_HASH:

		std::cout<<hasher.hash((int_16bit)32145,4)<<std::endl;
		std::cout<<hasher.hash((int_16bit)32145,5)<<std::endl;
		std::cout<<hasher.hash((int_16bit)32145,13)<<std::endl;
		
		if(turn--)
		{
			std::cout<<"second time"<<std::endl;
			hasher.set_result_range(0,(int_32bit)2294967296);		
			goto _BEGIN_HASH;
		}

	}
*/

};


#endif
