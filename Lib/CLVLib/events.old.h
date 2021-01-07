// events.h: interface for the sequencer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTS_H__5E3E9094_5A14_4A1A_8E61_539140149ECD__INCLUDED_)
#define AFX_EVENTS_H__5E3E9094_5A14_4A1A_8E61_539140149ECD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <algorithm>

namespace Utilities
{
	namespace CLV
	{
		/*** base class for handler wrapper instances ***/
		class delegate_base
		{
		public:
			delegate_base(void* pobject, void* pfn) { m_pObject = pobject; m_pFn = pfn; }
			delegate_base(const delegate_base& rhs) { m_pObject = rhs.m_pObject; m_pFn = rhs.m_pFn; }
			bool equals(const delegate_base& rhs) const { return m_pObject == rhs.m_pObject && m_pFn == rhs.m_pFn; }

		protected:
			void* GetFn() { return m_pFn; }
			void* GetObject() { return m_pObject; }

		private:
			void* m_pObject;
			void* m_pFn;
		};


		template <class A>
		class delegate1_base : public delegate_base
		{
		public:
			delegate1_base(void* pobject, void* pfn) : delegate_base(pobject, pfn) { ; }
			delegate1_base(const delegate1_base& rhs) : delegate_base(rhs) { ; }
			void operator() (A arg) { execute(arg); }
			virtual delegate1_base* Copy() = 0;

		protected:
			virtual void execute(A arg) = 0;
		};

		template <class A1, class A2>
		class delegate2_base : public delegate_base
		{
		public:
			delegate2_base(void* pobject, void* pfn) : delegate_base(pobject, pfn) { ; }
			delegate2_base(const delegate2_base& rhs) : delegate_base(rhs) { ; }
			void operator() (A1 arg1, A2 arg2) { execute(arg1, arg2); }
			virtual delegate2_base* Copy() = 0;

		protected:
			virtual void execute(A1 arg1, A2 arg2) = 0;
		};

		/*** handler wrapper for global and static functions  ***/
		template <class A>
		class delegate1_static : public delegate1_base<A>
		{
			typedef void(*FnPtr)(A);
		public:
			delegate1_static(FnPtr pFn) : delegate1_base(NULL, pFn) { ; }
			delegate1_static(const delegate1_static& rhs) : delegate1_base(rhs) { ; }
			virtual delegate1_base<A>* Copy() { return new delegate1_static(*this); }

		protected:
			virtual void execute(A arg)
			{
				FnPtr pfn = (FnPtr)GetFn();
				(*pfn)(arg);
			}
		};

		template <class A1, class A2>
		class delegate2_static : public delegate2_base<A1, A2>
		{
			typedef void(*FnPtr)(A1, A2);
		public:
			delegate2_static(FnPtr pFn) : delegate2_base(NULL, pFn) { ; }
			delegate2_static(const delegate2_static& rhs) : delegate2_base(rhs) { ; }
			virtual delegate2_base<A1, A2>* Copy() { return new delegate2_static(*this); }

		protected:
			virtual void execute(A1 arg1, A2 arg2)
			{
				FnPtr pfn = (FnPtr)GetFn();
				(*pfn)(arg1, arg2);
			}
		};

		/*** handler wrapper for non-static member functions  ***/
		// type of the object (O) differs from type to which member function belongd (T)
		// because that makes it possible to use member methods from base classes
		template <class O, class T, class A>
		class delegate1 : public delegate1_base<A>
		{
			typedef void (T::* FnPtr)(A);
		public:
			delegate1(O* pObj, FnPtr pFn) : delegate1_base(pObj, &pFn), m_pFn(pFn) { ; }
			delegate1(const delegate1& rhs) : delegate1_base(rhs) { m_pFn = rhs.m_pFn; }
			virtual delegate1_base* Copy() { return new delegate1(*this); }

		protected:
			virtual void execute(A arg)
			{
				O* pobj = (O*)GetObject();
				(pobj->*m_pFn)(arg);
			}

		private:
			FnPtr m_pFn;
		};

		template <class O, class T, class A1, class A2>
		class delegate2 : public delegate2_base<A1, A2>
		{
			typedef void (T::* FnPtr)(A1, A2);
		public:
			delegate2(O* pObj, FnPtr pFn) : delegate2_base(pObj, &pFn), m_pFn(pFn) { ; }
			delegate2(const delegate2& rhs) : delegate2_base(rhs) { m_pFn = rhs.m_pFn; }
			virtual delegate2_base<A1, A2>* Copy() { return new delegate2(*this); }

		protected:
			virtual void execute(A1 arg1, A2 arg2)
			{
				O* pobj = (O*)GetObject();
				(pobj->*m_pFn)(arg1, arg2);
			}

		private:
			FnPtr m_pFn;
		};

		/*** helper functions to instantiate handler ***/
		template <class O, class T, class A>
		delegate1<O, T, A> event_handler(O* pObj, void (T::* pFn)(A))
		{
			return delegate1<O, T, A>(pObj, pFn);
		}

		template <class A>
		delegate1_static<A> event_handler(void(*pFn)(A))
		{
			return delegate1_static<A>(pFn);
		}

		template <class O, class T, class A1, class A2>
		delegate2<O, T, A1, A2> event_handler(O* pObj, void (T::* pFn)(A1, A2))
		{
			return delegate2<O, T, A1, A2>(pObj, pFn);
		}

		template <class A1, class A2>
		delegate2_static<A1, A2> event_handler(void(*pFn)(A1, A2))
		{
			return delegate2_static<A1, A2>(pFn);
		}

		/*** helper class for searching through handler list ***/
		template <class T>
		class finder
		{
			const T* m_obj;
		public:
			finder(T* obj) : m_obj(obj) { ; }
			bool operator()(const T* obj) { return obj->equals(*m_obj); }
		};

		/*** event base implementation ***/
		template <class ARG>
		class event1_base
		{
		protected:
			typedef delegate1_base<ARG> Delegate;
			typedef Delegate* DelegatePtr;
			typedef std::list<DelegatePtr> DelegateList;
			typedef typename DelegateList::iterator DelegateIterator;
			typedef void(*FnPtrType)(ARG);

			DelegateList m_delegates;

		public:
			~event1_base()
			{
				for (DelegateIterator it = m_delegates.begin(); it != m_delegates.end(); ++it)
					delete *it;
			}

			void operator+= (Delegate& del)
			{
				finder<Delegate> f(&del);
				if (std::find_if(m_delegates.begin(), m_delegates.end(), f) != m_delegates.end())
				{
					return;
				}

				m_delegates.push_back(del.Copy());
			}

			void operator+= (FnPtrType pfn)
			{
				(*this) += event_handler(pfn);
			}

			void operator-= (Delegate& del)
			{
				finder<Delegate> f(&del);
				DelegateIterator it = std::find_if(m_delegates.begin(), m_delegates.end(), f);
				if (it != m_delegates.end())
				{
					delete *it;
					m_delegates.erase(it);
				}
			}

			void operator-= (FnPtrType pfn)
			{
				(*this) -= event_handler(pfn);
			}

			void operator() (ARG a)
			{
				for (DelegateIterator it = m_delegates.begin(); it != m_delegates.end(); ++it)
				{
					(*(*it))(a);
				}
			}

			size_t getSubscriberCount() const { return m_delegates.size(); }
		};


		template <class ARG1, class ARG2>
		class event2_base
		{
		protected:
			typedef delegate2_base<ARG1, ARG2> Delegate;
			typedef Delegate* DelegatePtr;
			typedef std::list<DelegatePtr> DelegateList;
			typedef typename DelegateList::iterator DelegateIterator;
			typedef void(*FnPtrType)(ARG1, ARG2);

			DelegateList m_delegates;

		public:
			~event2_base()
			{
				for (DelegateIterator it = m_delegates.begin(); it != m_delegates.end(); ++it)
					delete *it;
			}

			void operator+= (Delegate& del)
			{
				finder<Delegate> f(&del);
				if (std::find_if(m_delegates.begin(), m_delegates.end(), f) != m_delegates.end())
				{
					return;
				}

				m_delegates.push_back(del.Copy());
			}

			void operator+= (FnPtrType pfn)
			{
				(*this) += event_handler(pfn);
			}

			void operator-= (Delegate& del)
			{
				finder<Delegate> f(&del);
				DelegateIterator it = std::find_if(m_delegates.begin(), m_delegates.end(), f);
				if (it != m_delegates.end())
				{
					delete *it;
					m_delegates.erase(it);
				}
			}

			void operator-= (FnPtrType pfn)
			{
				(*this) -= event_handler(pfn);
			}

			void operator() (ARG1 a1, ARG2 a2)
			{
				for (DelegateIterator it = m_delegates.begin(); it != m_delegates.end(); ++it)
				{
					(*(*it))(a1, a2);
				}
			}

			size_t getSubscriberCount() const { return m_delegates.size(); }
		};

		/** typelist helpers **/
		template <class H, class T>
		struct typelist
		{
			typedef H head;
			typedef T tail;
		};

		class null_typelist {};

		/** event template **/
		template <class ARG1, class ARG2 = null_typelist>
		class event : public event2_base<ARG1, ARG2>
		{
		};

		template <class ARG>
		class event<ARG, null_typelist> : public event1_base<ARG> // partially specialized
		{
		};
	}
}

#endif // !defined(AFX_EVENTS_H__5E3E9094_5A14_4A1A_8E61_539140149ECD__INCLUDED_)
