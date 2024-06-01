#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <fstream>
#include <cctype>

namespace ca
{
	bool IsBinaryString(const std::string& str)
	{
		for(const char x : str)
		{
			if(x != '0' && x != '1')
			{
				return false;
			}
		}
		return !str.empty();
	}

	class Object 
	{
		public:
		virtual std::string ToString() const = 0;

		friend std::ostream& operator<<(std::ostream& out,const Object& obj)
		{
			out << obj.ToString();
			return out;
		}
	};

	class Block : public Object
	{
		private:
		std::string ToString() const override final
		{
			std::stringstream out;
			unsigned int i = Size();

			while(i != 0)
			{
				out << (Get(i-1)?('1'):('0'));
				i -= 1;
			}
			std::string str = out.str();
			return ((str.empty())?("0"):(str));
		}

		public:
		virtual unsigned int Size() const = 0;
		virtual bool Get(unsigned int) const = 0;
		virtual void Set(unsigned int,bool) = 0;
		
		friend unsigned int Value(const Block& itm) 
		{
			unsigned int index = 0;
			int n = itm.Size();

			while(n > 32 && !itm.Get(n))
			{
				n -= 1;
			}

			if(n > 32)
			{
				return index;
			}

			while(n != 0)
			{
				if(itm.Get(n-1))
				{
					index = ((1 << (n - 1)) ^ index);
				}
				n -= 1;
			}
			return index;
		}

		friend bool operator==(const Block& lhs,const Block& rhs)
		{
			unsigned int i = 0;

			while(i < lhs.Size() && i < rhs.Size())
			{
				if(lhs.Get(i) != rhs.Get(i)) {return false;}
				i += 1;
			}

			while(i < lhs.Size())
			{
				if(lhs.Get(i)) {return false;}
				i += 1;
			}

			while(i < rhs.Size())
			{
				if(rhs.Get(i)) {return false;}
				i += 1;
			}
			return true;
		}

		friend bool operator!=(const Block& lhs,const Block& rhs)
		{
			return !(lhs == rhs);
		}

	};

	class Iterator
	{
		private:
		Block* data;
		bool value;
		int index;
		bool current;

		public:
		Iterator() : Iterator(nullptr) {}

		Iterator(Block* data) : data(data)
		{
			current = false; 
			index = 0; 
		}

		Iterator(const Iterator& obj) {*this = obj;}
	
		Iterator& operator=(const Iterator& rhs)
		{
			if(this != &rhs)
			{
				data = rhs.data;
				current = false;
				index = rhs.index;
			}
			return *this;
		}

		~Iterator() {data = nullptr;}
		bool HasMove() const {return (data != nullptr && index >= 0 && index < data->Size());}

		bool& Value() 
		{
			if(data == nullptr || index < 0 || index >= data->Size())
			{
				throw std::invalid_argument("out of bound");
			}
			current = true;
			value = data->Get(index);
			return value;
		}

		bool Value() const
		{
			if(data == nullptr || index < 0 || index >= data->Size())
			{
				throw std::invalid_argument("out of bound");
			}
			return data->Get(index);
		}

		bool Sync() 
		{
			if(data != nullptr && current)
			{
				data->Set(index,value);
				current = false;
				return true;
			}
			return false;
		}

		Iterator& ToMost() 
		{
			if(data != nullptr)
			{
				current = false;
				index = data->Size()-1;
			}
			return *this;
		}

		Iterator& ToLeast()
		{
			if(data != nullptr)
			{
				current = false;
				index = 0;
			}
			return *this;
		}

		void operator++()
		{
			if(data != nullptr && index >= 0 && index < data->Size())
			{
				current = false;
				index += 1;
			}
		}

		void operator--()
		{
			if(data != nullptr && index >= 0 && index < data->Size())
			{
				current = false;
				index -= 1;
			}
		}

		void operator+=(unsigned int step)
		{
			if(data != nullptr && index >= 0 && index < data->Size())
			{
				current = false;
				index += step;
			}
		}

		void operator-=(unsigned int step)
		{
			if(data != nullptr && index >= 0 && index < data->Size())
			{
				current = false;
				index -= step;
			}
		}
	};

	class Couple
	{
		private:
		Couple(const Couple&) = delete;
		Couple& operator=(const Couple&) = delete;
		Block* data[2];

		public:
		Couple() : data{nullptr,nullptr} {}
       		~Couple() {data[0] = data[1] = nullptr;}
		void Clear() {data[0] = data[1] = nullptr;}
		Block* operator[](unsigned int idx) {return data[idx%2];}
		const Block* operator[](unsigned int idx) const {return data[idx%2];}
		bool HasCoupling() const {return (data[0] != nullptr && data[1] != nullptr);}

		bool Join(Block* lhs,Block* rhs)
		{
			if(lhs != rhs && lhs != nullptr && rhs != nullptr)
			{
				data[0] = lhs;
				data[1] = rhs;
				return true;
			}
			return false;
		}
	};

	class Mesh : public Block
	{
		private:
		unsigned int most[2];
		unsigned int ps;
		Couple cp;
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;


		public:
		Mesh() : most{0,0}, ps(0) {}
		Mesh(Block* lhs,Block* rhs) : ps(0) {Join(lhs,rhs);}
		~Mesh() {}

		unsigned int Size() const override 
		{
			if(!cp.HasCoupling() || most[0] >= cp[0]->Size() || most[1] >= cp[1]->Size())
			{
				return 0;
			}
			return (1 + ((most[0] <= most[1])?(most[0]):(most[1])));
		}

		bool Get(unsigned int idx) const override
		{
			unsigned int ln = Size(), n = 1 + most[ps] - ln;
			return ((idx >= ln)?(false):(cp[ps]->Get(n+idx)));
		}

		void Set(unsigned int idx,bool bit) override
		{
			unsigned int ln = Size();

			if(idx < ln)
			{
				unsigned int s = ln - 1;

				for(unsigned int i = 0;i < 2;i += 1)
				{
					cp[i]->Set(most[i]-s+idx,bit);
				}
			}
		}

		bool Join(Block* lhs,Block* rhs)
		{
			if(lhs != this && rhs != this && cp.Join(lhs,rhs))
			{
				most[0] = lhs->Size()-1;
				most[1] = rhs->Size()-1;
				return true;
			}
			cp.Clear();
			return false;
		}
		
		bool Join(Block* lhs,unsigned int lmi,Block* rhs,unsigned int rmi)
		{
			if(lhs != this && rhs != this && cp.Join(lhs,rhs) && SetBoth(lmi,rmi))
			{
				return true;
			}
			cp.Clear();
			return false;
		}

		bool SetBoth(unsigned int lhs,unsigned int rhs)
		{
			if(cp.HasCoupling() && lhs < cp[0]->Size() && rhs < cp[1]->Size())
			{
				most[0] = lhs;
				most[1] = rhs;
				return true;
			}
			return false;
		}

		bool SetMost(bool idx,unsigned int ms)
		{
			unsigned int i = ((idx)?(0):(1));
			
			if(cp.HasCoupling() && ms < cp[i]->Size())
			{
				most[i] = ms;
				return true;
			}
			return false;
		}

		bool Sync(bool dir,unsigned int len)
		{
			if(cp.HasCoupling() && len != 0 && len <= Size())
			{
				unsigned int s = len - 1, p = ((dir)?(0):(1)), q = (p + 1) % 2;

				for(unsigned int i = 0;i < len;i += 1)
				{
					cp[q]->Set(most[q]-s+i,cp[p]->Get(most[p]-s+i));
				}
				return true;
			}
			return false;
		}
		
		bool Sync(bool dir) {return Sync(dir,Size());}

		bool HasConnection() const {return cp.HasCoupling();}

		void SetView(bool dir) {ps = ((dir)?(0):(1));}
	};

	class Link : public Block
	{
		private:
		Couple cp;
		Link(const Link&) = delete;
		Link& operator=(const Link&) = delete;
	
		public:
		Link() {}
		Link(Block* lhs,Block* rhs) {Join(lhs,rhs);}
		~Link() {}

		unsigned int Size() const override 
		{
			return ((!cp.HasCoupling())?(0):(cp[0]->Size()+cp[1]->Size()));
		}

		bool Get(unsigned int idx) const override
		{
			if(idx >= Size()) {return false;}
			else if(idx >= cp[1]->Size()) {return cp[0]->Get(idx-cp[1]->Size());}
			else {return cp[1]->Get(idx);}
		}

		void Set(unsigned int idx,bool bit) override
		{
			if(idx < Size())
			{
				if(idx >= cp[1]->Size()) {cp[0]->Set(idx-cp[1]->Size(),bit);}
				else {cp[1]->Set(idx,bit);}
			}
		}

		bool Join(Block* lhs,Block* rhs)
		{
			return (lhs != this && rhs != this && cp.Join(lhs,rhs));
		}

		bool HasConnection() const {return cp.HasCoupling();}
	};
 
	class Word : public Block
	{
		protected:
		std::string content;

		public:
		Word() : Word(32) {}

		Word(unsigned int size,bool value = false)
		{
			content = std::string(((size == 0)?(32):(size)),((value)?('1'):('0')));
		}

		Word(std::string word) : Word() 
		{
	       	if(IsBinaryString(word)) {content = word;}
		}

		Word(const Block& obj) {*this = obj;} 

		Word(const Word& obj) {*this = obj;}

		Word& operator=(const Word& rhs)
		{
			if(this != &rhs) {content = rhs.content;}
			return *this;
		}

		Word& operator=(const Block& rhs)
		{
			std::stringstream out;
			out << rhs;
			content = out.str();
			return *this;
		}

		Word& operator=(const bool rhs)
		{
			content = ((rhs)?('1'):('0'));
			return *this;
		}

		~Word() {}
		
		unsigned int Size() const override {return content.size();}

		bool Get(unsigned int idx) const override
		{
			unsigned int n = content.size() - 1;
			return ((idx >= content.size())?(false):(content[n-idx]=='1'));
		}

		void Set(unsigned int idx,bool bit) override
		{
			unsigned int n = (content.size() - 1) - idx;

			if(idx < content.size() && (content[n] == '1') != bit)
			{
				content[n] = (bit)?('1'):('0');
			}
		}

		Word operator()(unsigned int i,unsigned int j) const
		{
			if(i < content.size() && j < content.size())
			{
				unsigned int mn = (i < j)?(i):(j);
			       	unsigned int mx = (i + j - mn); 
				unsigned int ln = (i + j - 2 * mn + 1);
				return Word(content.substr(((content.size()-1)-mx),ln));
			}
			return Word(1);
		}

		void Zero() {content = std::string(content.size(),'0');}

		friend Word operator+(const Word& lhs,const Word& rhs)
		{
			return Word(lhs.content+rhs.content);
		}
	};			

	class EventLog
	{
		private:
		static std::string records;

		public:
		static void AddEvent(const std::string& id,const Block& data)
		{
			std::stringstream out;
			out << std::left << std::setw(10) << id.substr(0,10) << "   " << data << "\n";
			records += out.str();
		}

		static void Download(const std::string& filename)
		{
			std::fstream out(filename.data(),std::fstream::out);
			out << records;
			out.close();
		}

		static std::string ViewLog() {return records;}
		static void ClearLog() {records = std::string();}		
	};

	std::string EventLog::records = std::string();

	enum class Group {Data, Address, Control};

	class BusComponent : public Block
	{
		public:
		virtual unsigned int Lines(Group) const = 0;
		virtual unsigned int Most(Group) const = 0;
		virtual void Reset() = 0;
	};

	class ExecutableComponent 
	{
		public:
		virtual void Process() = 0;
		virtual void Connect(BusComponent*) = 0;
		virtual bool CanExecute() const = 0;
	};

	class MemoryComponent : public ExecutableComponent
	{
		public:
		virtual void Read() = 0;
		virtual void Write() = 0;
		virtual unsigned int Size() const = 0;
	};

	class ProcessorComponent : public ExecutableComponent 
	{
		public:
		virtual void ALU() = 0;
		virtual void CU() = 0;
	};

	class CompilerComponent : public ExecutableComponent
	{
		public:
		virtual void Load(const std::string&) = 0;
		virtual void Link(MemoryComponent*) = 0;
		virtual bool Compile() = 0;
	};
}

#endif
