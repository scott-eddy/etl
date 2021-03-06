/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
http://www.etlcpp.com

Copyright(c) 2014 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "UnitTest++.h"
#include "ExtraCheckMacros.h"

#include "data.h"

#include <set>
#include <vector>
#include <string>

#include "pool.h"
#include "largest.h"

#if defined(ETL_COMPILER_GCC)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

typedef TestDataDC<std::string>  Test_Data;
typedef TestDataNDC<std::string> Test_Data2;

namespace
{
  struct D0
  {
    D0()
    {
    }
  };

  struct D1
  {
    D1(const std::string& a_)
      : a(a_)
    {
    }

    std::string a;
  };

  struct D2
  {
    D2(const std::string& a_, const std::string& b_)
      : a(a_),
      b(b_)
    {
    }

    std::string a;
    std::string b;
  };

  struct D3
  {
    D3(const std::string& a_, const std::string& b_, const std::string& c_)
      : a(a_),
      b(b_),
      c(c_)
    {
    }

    std::string a;
    std::string b;
    std::string c;
  };

  struct D4
  {
    D4(const std::string& a_, const std::string& b_, const std::string& c_, const std::string& d_)
      : a(a_),
      b(b_),
      c(c_),
      d(d_)
    {
    }

    std::string a;
    std::string b;
    std::string c;
    std::string d;
  };

  bool operator == (const D0&, const D0&)
  {
    return true;
  }

  bool operator == (const D1& lhs, const D1& rhs)
  {
    return (lhs.a == rhs.a);
  }

  bool operator == (const D2& lhs, const D2& rhs)
  {
    return (lhs.a == rhs.a) && (lhs.b == rhs.b);
  }

  bool operator == (const D3& lhs, const D3& rhs)
  {
    return (lhs.a == rhs.a) && (lhs.b == rhs.b) && (lhs.c == rhs.c);
  }

  bool operator == (const D4& lhs, const D4& rhs)
  {
    return (lhs.a == rhs.a) && (lhs.b == rhs.b) && (lhs.c == rhs.c) && (lhs.d == rhs.d);
  }

  std::ostream& operator <<(std::ostream& os, const D0&)
  {
     return os;
  }

  std::ostream& operator <<(std::ostream& os, const D1& d)
  {
    os << d.a;
    return os;
  }

  std::ostream& operator <<(std::ostream& os, const D2& d)
  {
    os << d.a << " " << d.b;
    return os;
  }

  std::ostream& operator <<(std::ostream& os, const D3& d)
  {
    os << d.a << " " << d.b << " " << d.c;
    return os;
  }

  std::ostream& operator <<(std::ostream& os, const D4& d)
  {
    os << d.a << " " << d.b << " " << d.c << " " << d.d;
    return os;
  }

  SUITE(test_pool)
  {
    //*************************************************************************
    TEST(test_allocate)
    {
      etl::pool<Test_Data, 4> pool;

      Test_Data* p1 = nullptr;
      Test_Data* p2 = nullptr;
      Test_Data* p3 = nullptr;
      Test_Data* p4 = nullptr;

      CHECK_NO_THROW(p1 = pool.allocate<Test_Data>());
      CHECK_NO_THROW(p2 = pool.allocate<Test_Data>());
      CHECK_NO_THROW(p3 = pool.allocate<Test_Data>());
      CHECK_NO_THROW(p4 = pool.allocate<Test_Data>());

      CHECK(p1 != p2);
      CHECK(p1 != p3);
      CHECK(p1 != p4);
      CHECK(p2 != p3);
      CHECK(p2 != p4);
      CHECK(p3 != p4);

      CHECK_THROW(pool.allocate<Test_Data>(), etl::pool_no_allocation);
    }

    //*************************************************************************
    TEST(test_release)
    {
      etl::pool<Test_Data, 4> pool;

      Test_Data* p1 = pool.allocate<Test_Data>();
      Test_Data* p2 = pool.allocate<Test_Data>();
      Test_Data* p3 = pool.allocate<Test_Data>();
      Test_Data* p4 = pool.allocate<Test_Data>();

      CHECK_NO_THROW(pool.release(p2));
      CHECK_NO_THROW(pool.release(p3));
      CHECK_NO_THROW(pool.release(p1));
      CHECK_NO_THROW(pool.release(p4));

      CHECK_EQUAL(4U, pool.available());

      Test_Data not_in_pool;

      CHECK_THROW(pool.release(&not_in_pool), etl::pool_object_not_in_pool);
    }

    //*************************************************************************
    TEST(test_allocate_release)
    {
      etl::pool<Test_Data, 4> pool;

      Test_Data* p1 = pool.allocate<Test_Data>();
      Test_Data* p2 = pool.allocate<Test_Data>();
      Test_Data* p3 = pool.allocate<Test_Data>();
      Test_Data* p4 = pool.allocate<Test_Data>();

      // Allocated p1, p2, p3, p4

      CHECK_EQUAL(0U, pool.available());

      CHECK_NO_THROW(pool.release(p2));
      CHECK_NO_THROW(pool.release(p3));

      // Allocated p1, p4

      CHECK_EQUAL(2U, pool.available());

      Test_Data* p5 = pool.allocate<Test_Data>();
      Test_Data* p6 = pool.allocate<Test_Data>();

      // Allocated p1, p4, p5, p6

      CHECK_EQUAL(0U, pool.available());

      CHECK(p5 != p1);
      CHECK(p5 != p4);

      CHECK(p6 != p1);
      CHECK(p6 != p4);

      CHECK_NO_THROW(pool.release(p5));

      // Allocated p1, p4, p6

      CHECK_EQUAL(1U, pool.available());

      Test_Data* p7 = pool.allocate<Test_Data>();

      // Allocated p1, p4, p6, p7

      CHECK(p7 != p1);
      CHECK(p7 != p4);
      CHECK(p7 != p6);

      CHECK(pool.full());
    }

    //*************************************************************************
    TEST(test_available)
    {
      etl::pool<Test_Data, 4> pool;
      CHECK_EQUAL(4U, pool.available());

      Test_Data* p;

      p = pool.allocate<Test_Data>();
      CHECK_EQUAL(3U, pool.available());

      p = pool.allocate<Test_Data>();
      CHECK_EQUAL(2U, pool.available());

      p = pool.allocate<Test_Data>();
      CHECK_EQUAL(1U, pool.available());

      p = pool.allocate<Test_Data>();
      CHECK_EQUAL(0U, pool.available());
    }

    //*************************************************************************
    TEST(test_max_size)
    {
      etl::pool<Test_Data, 4> pool;

      CHECK(pool.max_size() == 4U);
    }

    //*************************************************************************
    TEST(test_size)
    {
      etl::pool<Test_Data, 4> pool;
      CHECK_EQUAL(0U, pool.size());

      Test_Data* p;

      p = pool.allocate<Test_Data>();
      CHECK_EQUAL(1U, pool.size());

      p = pool.allocate<Test_Data>();
      CHECK_EQUAL(2U, pool.size());

      p = pool.allocate<Test_Data>();
      CHECK_EQUAL(3U, pool.size());

      p = pool.allocate<Test_Data>();
      CHECK_EQUAL(4U, pool.size());
    }

    //*************************************************************************
    TEST(test_empty_full)
    {
      etl::pool<Test_Data, 4> pool;
      CHECK(pool.empty());
      CHECK(!pool.full());

      Test_Data* p;

      p = pool.allocate<Test_Data>();
      CHECK(!pool.empty());
      CHECK(!pool.full());

      p = pool.allocate<Test_Data>();
      CHECK(!pool.empty());
      CHECK(!pool.full());

      p = pool.allocate<Test_Data>();
      CHECK(!pool.empty());
      CHECK(!pool.full());

      p = pool.allocate<Test_Data>();
      CHECK(!pool.empty());
      CHECK(pool.full());
    }

    //*************************************************************************
    TEST(test_is_in_pool)
    {
      etl::pool<Test_Data, 4> pool;
      Test_Data not_in_pool;

      Test_Data* p1 = pool.allocate<Test_Data>();

      CHECK(pool.is_in_pool(p1));
      CHECK(!pool.is_in_pool(&not_in_pool));
    }

    //*************************************************************************
    TEST(test_generic_storage)
    {
      union Storage
      {
        uint64_t dummy; // For alignment purposes.
        char buffer[1000];
      };

      etl::pool<Storage, 4> pool;

      Test_Data* pdata = pool.allocate<Test_Data>();
      new (pdata) Test_Data("ABC", 3);

      etl::array<int, 10>* parray = pool.allocate<etl::array<int, 10>>();
      new (parray) etl::array<int, 10>();
      parray->fill(0x12345678);

      etl::array<int, 10> compare;
      compare.fill(0x12345678);

      CHECK(pdata->value == "ABC");
      CHECK(pdata->index == 3);
      CHECK(*parray == compare);

      pool.release(parray);
      pool.release(pdata);

      CHECK_EQUAL(4U, pool.available());
    }

    //*************************************************************************
    TEST(test_type_error)
    {
      struct Test
      {
        uint64_t a;
        uint64_t b;
      };

      etl::pool<uint32_t, 4> pool;

      etl::ipool& ip = pool;

      CHECK_THROW(ip.allocate<Test>(), etl::pool_element_size);
    }

    //*************************************************************************
    TEST(test_generic_allocate)
    {
      typedef etl::largest<uint8_t, uint32_t, double, Test_Data> largest;

      etl::generic_pool<largest::size, largest::alignment, 4> pool;

      uint8_t*   p1 = nullptr;
      uint32_t*  p2 = nullptr;
      double*    p3 = nullptr;
      Test_Data* p4 = nullptr;

      CHECK_NO_THROW(p1 = pool.allocate<uint8_t>());
      CHECK_NO_THROW(p2 = pool.allocate<uint32_t>());
      CHECK_NO_THROW(p3 = pool.allocate<double>());
      CHECK_NO_THROW(p4 = pool.allocate<Test_Data>());
    }
  };

  //*************************************************************************
  TEST(test_create_destroy)
  {
    etl::pool<D0, 4> pool0;
    etl::pool<D1, 4> pool1;
    etl::pool<D2, 4> pool2;
    etl::pool<D3, 4> pool3;
    etl::pool<D4, 4> pool4;

    D0* p0 = pool0.create<D0>();
    D1* p1 = pool1.create<D1>("1");
    D2* p2 = pool2.create<D2>("1", "2");
    D3* p3 = pool3.create<D3>("1", "2", "3");
    D4* p4 = pool4.create<D4>("1", "2", "3", "4");

    CHECK_EQUAL(pool0.max_size() - 1, pool0.available());
    CHECK_EQUAL(1U, pool0.size());

    CHECK_EQUAL(pool1.max_size() - 1, pool1.available());
    CHECK_EQUAL(1U, pool1.size());

    CHECK_EQUAL(pool2.max_size() - 1, pool2.available());
    CHECK_EQUAL(1U, pool2.size());

    CHECK_EQUAL(pool3.max_size() - 1, pool3.available());
    CHECK_EQUAL(1U, pool3.size());

    CHECK_EQUAL(pool4.max_size() - 1, pool4.available());
    CHECK_EQUAL(1U, pool4.size());

    CHECK_EQUAL(D0(), *p0);
    CHECK_EQUAL(D1("1"), *p1);
    CHECK_EQUAL(D2("1", "2"), *p2);
    CHECK_EQUAL(D3("1", "2", "3"), *p3);
    CHECK_EQUAL(D4("1", "2", "3", "4"), *p4);

    pool0.destroy<D0>(p0);
    pool1.destroy<D1>(p1);
    pool2.destroy<D2>(p2);
    pool3.destroy<D3>(p3);
    pool4.destroy<D4>(p4);

    CHECK_EQUAL(pool0.max_size(), pool0.available());
    CHECK_EQUAL(0U, pool0.size());

    CHECK_EQUAL(pool1.max_size(), pool1.available());
    CHECK_EQUAL(0U, pool1.size());

    CHECK_EQUAL(pool2.max_size(), pool2.available());
    CHECK_EQUAL(0U, pool2.size());

    CHECK_EQUAL(pool3.max_size(), pool3.available());
    CHECK_EQUAL(0U, pool3.size());

    CHECK_EQUAL(pool4.max_size(), pool4.available());
    CHECK_EQUAL(0U, pool4.size());
  }
}

#if defined(ETL_COMPILER_GCC)
  #pragma GCC diagnostic pop
#endif
