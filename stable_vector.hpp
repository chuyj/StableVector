#ifndef STABLE_VECTOR_HPP
#define STABLE_VECTOR_HPP

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <iostream>
using namespace std;

template<typename T>
class stable_vector {
	private:
		struct node;

	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef std::size_t size_type; 
		typedef std::ptrdiff_t difference_type; 

		class iterator;
		class const_iterator;
		
		stable_vector():v(1, new node){
			v.front() -> up = v.begin();
		}

		explicit stable_vector(const size_type n, const T& value = T()):v(n + 1){
			for (size_type i = 0; i < n + 1; i++){
				v[i] = new node;
				v[i] -> datum = value;
				v[i] -> up = v.begin() + i;
			}
		}

		template<typename InputIterator>
		stable_vector(InputIterator first, InputIterator last, typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = nullptr):v(last - first + 1){
			int i = 0;
			for (InputIterator it = first; it != last; it++, i++){
				v[i] = new node;
				v[i] -> datum = *it;
				v[i] -> up = v.begin() + i;
			}
			v[i] = new node;
			v[i] -> up = v.begin() + i;
		}

		stable_vector(const stable_vector& rhs){
			(*this) = rhs;
		}

		stable_vector& operator=(const stable_vector& rhs){
			clear();
			delete v.front();
			v = rhs.v;
			for (size_type i = 0; i < rhs.size(); i++){
				v[i] = new node;
				v[i] -> datum = rhs[i];
				v[i] -> up = v.begin() + i;
			}
			v.back() = new node;
			v.back() -> up = v.end() - 1;
			return (*this);
		}

		~stable_vector(){
			clear();
			delete v.front();
		}
		void assign(const size_type n, const T& value){
			(*this) = stable_vector(n, value);
		}
		template<typename InputIterator>
		void assign(InputIterator first, InputIterator last, typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = nullptr){
			(*this) = stable_vector(first, last);
		}

		reference at(const size_type pos) { return pos < size() ? (*this)[pos] : throw std::range_error("stable_vector: out of range"); }
		const_reference at(const size_type pos) const { return pos < size() ? (*this)[pos] : throw std::range_error("stable_vector: out of range"); }

		reference operator[](const size_type pos){
			return v[pos] -> datum;
		}
		const_reference operator[](const size_type pos) const{
			return v[pos] -> datum;
		}

		reference front(){
			return v.front() -> datum;
		}
		const_reference front() const{
			return v.front() -> datum;
		}

		reference back(){
			return v[size() - 1] -> datum;
		}
		const_reference back() const{
			return v[size() - 1] -> datum;
		}

		iterator begin(){return iterator(v.front());}
		const_iterator begin() const{return const_iterator(v.front());}
		const_iterator cbegin() const{return const_iterator(v.front());}

		iterator end(){return iterator(v.back());}
		const_iterator end() const{return const_iterator(v.back());}
		const_iterator cend() const{return const_iterator(v.back());}

		bool empty() const{
			return size() == 0;
		}

		size_type size() const{
			return v.size() - 1;
		}

		void clear(){
			for (size_type i = 0; i < v.size(); i++)
				delete v[i];
			v.clear();
			v.push_back(new node);
			v.front() -> up = v.begin();
			return ;
		}
		iterator insert(const_iterator pos, const T& value){
			node* ins = new node;
			ins -> datum = value;
			ins -> up = v.insert(pos.n -> up, ins);
			for (typename vector_type::iterator it = (v.front() -> up == v.begin() ? ins -> up : v.begin()); it != v.end(); it++)
				(*it) -> up = it;
			return iterator(ins);
			// typename vector_type::iterator it;
			// it = pos.n -> up;
			// v.insert(it, new node);
			// typename vector_type::iterator reit = it;
			// (*it) -> datum = value;
			// if (v.begin() != v.front() -> up)
			//	 it = v.begin();
			// for (; it != v.end(); it++){
			//	 (*it) -> up = it;
			// }
			// return iterator(*reit);
		}
		template<typename InputIterator>
		iterator insert(const_iterator pos, InputIterator first, InputIterator last){
			stable_vector tmp(first, last);
			difference_type dif = last - first;
			difference_type ibegin = pos - begin();
			iterator it = tmp.begin();
			difference_type ii = ibegin;
			// for (difference_type i = 0; i < dif; i++, it++, ii++){
			// 	v.insert(v.begin() + ibegin, new node);
			// 	v[ii] -> datum = *it;
			// }
			//.cout << "done" << endl;
			node* np = new node;
			v.insert(v.begin() + ibegin, dif, np);
			for (difference_type i = 0; i < dif; i++, ii++, it++){
				v[ii] = new node;
				v[ii] -> datum = *it;
			}
			delete np;
			typename vector_type::iterator itt;
			if (v.front() -> up == v.begin()){
				itt = v.begin() + ibegin;
			}
			else {
				itt = v.begin();
			}
			for (; itt != v.end(); itt++){
				(*itt) -> up = itt;
			}
			return iterator(v[ibegin]);
			// difference_type i = ibegin;
			// for (iterator it = tmp.begin(); it != tmp.end(); it++, i++){
			// 	v[i] -> datum = *it;
			// }
			
			// stable_vector tmp(first, last);

			// difference_type n = pos - begin();
			// for (iterator it = tmp.begin(); it != tmp.end(); it++){
			// 	pos = insert(pos,*it) + 1;
			// }
			// return iterator(v[n]);
		}

		iterator erase(const_iterator pos){
			typename vector_type::iterator vret = v.erase(pos.n -> up);
			iterator ret(*vret);
			ret.n -> up = vret;
			typename vector_type::iterator it;
			if (v.front() -> up == v.begin()){
				it = ret.n -> up;
			}
			else{
				it = v.begin();
			}
			for (; it != v.end(); it++)
				(*it) -> up = it;
			delete pos.n;
			return ret;
		}
		iterator erase(const_iterator first, const_iterator last){
			difference_type first_ = first - begin();
			difference_type last_  = last - begin();
			for (typename vector_type::iterator it = v.begin() + first_; it != v.begin() + last_ ; it++){
				delete *it;
			}
			v.erase(v.begin() + first_, v.begin() +last_);
			for (typename vector_type::iterator it = v.begin() + first_; it != v.end(); it++){
				(*it) -> up = it;
			}
			return iterator(v[first_]);

			// difference_type num = last - first;
			// iterator re((node*)first.n);
			// while(num--){
			// 	re = erase(re);
			// }
			// return re;
		}

		void push_back(const T& value){
			insert(end(), value);
			return ;
		}
		void pop_back(){
			erase(end() - 1);
		}

		void resize(size_type count, const T& value = T()){
			while (size() < count) push_back(value);
			while (size() > count) pop_back();
			// size_type size_ = size();
			// if (size_ > count){
			// 	size_type n = size_ - count;
			// 	for (size_type i = 0; i < n; i++){
			// 		pop_back();
			// 	}
			// }
			// else if (size_ < count){
			// 	size_type n = count - size_;
			// 	for (size_type i = 0; i < n; i++){
			// 		push_back(value);
			// 	}
			// }
			// else if (size_ == count){
			// 	return ;
			// }
			// return ;			
		}

		void swap(stable_vector& other){
			std::swap(v, other.v);
            for (size_t i = 0; i < v.size(); i++){
                v[i] -> up = v.begin() + i;
            }
            for (size_t i = 0; i < other.v.size(); i++){
                other.v[i] -> up = other.v.begin() + i;
            }
            return ;
		}

		friend bool operator==(const stable_vector& lhs, const stable_vector& rhs){ return !(lhs != rhs); }
		friend bool operator!=(const stable_vector& lhs, const stable_vector& rhs){ return lhs > rhs || lhs < rhs; }
		friend bool operator< (const stable_vector& lhs, const stable_vector& rhs){ return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); }
		friend bool operator<=(const stable_vector& lhs, const stable_vector& rhs){ return !(lhs > rhs); }
		friend bool operator> (const stable_vector& lhs, const stable_vector& rhs){ return rhs < lhs; }
		friend bool operator>=(const stable_vector& lhs, const stable_vector& rhs){ return !(lhs < rhs); }

		class iterator {
			friend class stable_vector;

			public:
				typedef stable_vector::difference_type difference_type;
				typedef stable_vector::value_type value_type;
				typedef stable_vector::pointer pointer;
				typedef stable_vector::reference reference;
				typedef std::random_access_iterator_tag iterator_category;

				explicit iterator(node* const n_ = nullptr):n(n_){}
				iterator(const iterator& rhs):n(rhs.n){}
				iterator& operator=(const iterator& rhs){n = rhs.n;return (*this);}
				~iterator(){}

				reference operator*() const{return n -> datum;}
				pointer operator->() const { return std::addressof(operator*()); }

				friend iterator operator+(iterator it, const difference_type i){return iterator(*(it.n -> up + i));}
				friend iterator operator+(const difference_type i, iterator it){return it + i;}
				friend iterator operator-(iterator it, const difference_type i){return it + (-i);}
				friend difference_type operator-(const iterator lhs, const iterator rhs){return abs(lhs.n -> up - rhs.n -> up);}

				iterator& operator+=(const difference_type i){return (*this) = (*this) + i;}
				iterator& operator-=(const difference_type i){return (*this) = (*this) - i;}

				iterator& operator++(){
					return (*this) = (*this) + 1;
				}
				iterator operator++(int){
					iterator it = (*this);
					++(*this);
					return it;
				}

				iterator& operator--(){
					return (*this) = (*this) - 1;
				}
				iterator operator--(int){
					iterator it = (*this);
					--(*this);
					return it;
				}

				reference operator[](const difference_type n){return ((*this) + n).n -> datum;}
				const_reference operator[](const difference_type n) const{return ((*this) + n).n -> datum;}

				operator const_iterator() const{return const_iterator(n);}

				friend bool operator==(const iterator lhs, const iterator rhs){return !(lhs != rhs); }
				friend bool operator!=(const iterator lhs, const iterator rhs){return lhs > rhs || lhs < rhs; }
				friend bool operator< (const iterator lhs, const iterator rhs){return lhs.n -> up < rhs.n -> up; }
				friend bool operator<=(const iterator lhs, const iterator rhs){return !(lhs > rhs); }
				friend bool operator> (const iterator lhs, const iterator rhs){return rhs < lhs; }
				friend bool operator>=(const iterator lhs, const iterator rhs){return !(lhs < rhs); }

			private:
				node* n;
		};

		class const_iterator {
			friend class stable_vector;

			public:
				typedef stable_vector::difference_type difference_type;
				typedef stable_vector::value_type value_type;
				typedef stable_vector::pointer pointer;
				typedef stable_vector::reference reference;
				typedef std::random_access_iterator_tag iterator_category;

				explicit const_iterator(const node* const n_):n(n_){}
				const_iterator(const const_iterator& rhs):n(rhs.n){}
				const_iterator& operator=(const const_iterator& rhs){n = rhs.n; return (*this);}
				~const_iterator(){}

				friend const_iterator operator+(const_iterator it, const difference_type i){ return const_iterator(*(it.n -> up + i)); }
				friend const_iterator operator+(const difference_type i, const_iterator it){ return it + i; }
				friend const_iterator operator-(const_iterator it, const difference_type i){ return it + (-i); }
				friend difference_type operator-(const const_iterator lhs, const const_iterator rhs){ return abs(lhs.n -> up - rhs.n -> up); }

				const_iterator& operator+=(const difference_type i){ return (*this) = (*this) + i; }
				const_iterator& operator-=(const difference_type i){ return (*this) = (*this) - i; }
				
				const_reference operator*() const{return n -> datum;}
				const_pointer operator->() const { return std::addressof(operator*()); }

				const_iterator& operator++(){
					return (*this) += 1;
				}
				const_iterator operator++(int){
					const_iterator ret = (*this);
					++(*this);
					return ret;
				}

				const_iterator& operator--(){
					return (*this) -= 1;
				}
				const_iterator operator--(int){
					const_iterator ret = (*this);
					--(*this);
					return ret;
				}

				const_reference operator[](const difference_type i) const{return *((*this) + i);}

				friend bool operator==(const const_iterator lhs, const const_iterator rhs){return !(lhs != rhs);}
				friend bool operator!=(const const_iterator lhs, const const_iterator rhs){return lhs > rhs || lhs < rhs;}
				friend bool operator< (const const_iterator lhs, const const_iterator rhs){return lhs.n -> up< rhs.n -> up;}
				friend bool operator<=(const const_iterator lhs, const const_iterator rhs){return !(lhs > rhs);}
				friend bool operator> (const const_iterator lhs, const const_iterator rhs){return rhs < lhs;}
				friend bool operator>=(const const_iterator lhs, const const_iterator rhs){return !(lhs < rhs);}

			private:
				const node* n;
		};

	private:
		typedef std::vector<node*> vector_type;
		vector_type v;

		struct node {
			T datum;
			typename vector_type::iterator up;
		};
};

#endif
