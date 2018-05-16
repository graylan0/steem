#include <boost/test/unit_test.hpp>

#include "../concatenation_enumerator_tests/concatenation_enumerator_tests_mgr.hpp"
#include <chainbase/util/concatenation_enumerator.hpp>

#include <chrono>
#include <type_traits>

using namespace chainbase;

template< typename Collection, typename Object >
void basic_test()
{
   int cnt;

   Collection s1;
   Collection s2;

   auto p1 = std::make_tuple( &s1 );
   auto p2 = std::make_tuple( &s2 );

   BOOST_TEST_MESSAGE( "1 empty source" );
   cnt = 0;
   using Iterator = ce::concatenation_iterator< Object, ce_tests::cmp1 >;
   Iterator it01( ce_tests::cmp1(), p1 );
   Iterator it_end_01 = Iterator::create_end( ce_tests::cmp1(), p1 );
   while( it01 != it_end_01 )
   {
      ++it01;
      ++cnt;
   }
   BOOST_REQUIRE( cnt == 0 );

   BOOST_TEST_MESSAGE( "2 empty sources" );
   cnt = 0;
   Iterator it02( ce_tests::cmp1(), p1, p2 );
   Iterator it_end_02 = Iterator::create_end( ce_tests::cmp1(), p1, p2 );
   while( it02 != it_end_02 )
   {
      ++it02;
      ++cnt;
   }
   BOOST_REQUIRE( cnt == 0 );

   BOOST_TEST_MESSAGE( "1 filled source" );
   ce_tests::fill1< Object >( s1 );
   Iterator it03( ce_tests::cmp1(), std::make_tuple( &s1 ) );
   Iterator it_end_03 = Iterator::create_end( ce_tests::cmp1(), std::make_tuple( &s1 ) );
   auto it_src03 = s1.begin();
   while( it03 != it_end_03 )
   {
      BOOST_REQUIRE( ( *it03 ) == ( *it_src03 ) );

      ++it03;
      ++it_src03;
   }

   BOOST_TEST_MESSAGE( "2 filled sources: one has content, second is empty" );
   ce_tests::fill1< Object >( s1 );
   Iterator it04 (  ce_tests::cmp1(),
                     std::make_tuple( &s1 ),
                     std::make_tuple( &s2 )
                  );

   Iterator it_end_04 = Iterator::create_end( ce_tests::cmp1(),
                                                std::make_tuple( &s1 ),
                                                std::make_tuple( &s2 )
                                             );
   auto it_src04_1 = s1.begin();
   while( it04 != it_end_04 )
   {
      BOOST_REQUIRE( ( *it04 ) == ( *it_src04_1 ) );

      ++it04;
      ++it_src04_1;
   }

   BOOST_TEST_MESSAGE( "2 filled sources with the same content" );
   ce_tests::fill1< Object >( s1 );
   ce_tests::fill1< Object >( s2 );
   Iterator it05 ( ce_tests::cmp1(),
                     std::make_tuple( &s1 ),
                     std::make_tuple( &s2 )
                  );
   Iterator it_end_05 = Iterator::create_end( ce_tests::cmp1(),
                           std::make_tuple( &s1 ),
                           std::make_tuple( &s2 )
                        );
   auto it_src05_1 = s1.begin();
   auto it_src05_2 = s2.begin();
   while( it05 != it_end_05 )
   {
      BOOST_REQUIRE( ( *it05 ) == ( *it_src05_1 ) );
      BOOST_REQUIRE( ( *it05 ) == ( *it_src05_2 ) );

      ++it05;
      ++it_src05_1;
      ++it_src05_2;
   }

   BOOST_TEST_MESSAGE( "2 filled sources with different content" );
   ce_tests::fill2< Object >( s1 );
   ce_tests::fill2a< Object >( s2 );
   Iterator it06 ( ce_tests::cmp1(),
                     std::make_tuple( &s1 ),
                     std::make_tuple( &s2 )
                  );
   Iterator it_end_06 = Iterator::create_end( ce_tests::cmp1(),
                        std::make_tuple( &s1 ),
                        std::make_tuple( &s2 )
                     );
   cnt = 0;
   while( it06 != it_end_06 )
   {
      if( cnt == 0 )
         BOOST_REQUIRE( ( *it06 ) == ( *s1.begin() ) );
      else if( cnt == 1 )
         BOOST_REQUIRE( ( *it06 ) == ( *s2.begin() ) );
      else
         BOOST_REQUIRE( 0 && "Unknown state of enumerator" );

      ++it06;
      ++cnt;
   }
}

template< typename Collection1, typename Collection2, typename Index, typename Object, typename Cmp, typename SortedCollection >
void test_different_sources( const SortedCollection& sorted )
{
   Collection1 bmic1;
   Collection1 bmic2;
   Collection2 s1;

   BOOST_TEST_MESSAGE( "3 filled sources with different content" );
   ce_tests::fill1< Object >( bmic1 );
   ce_tests::fill2< Object >( bmic2 );
   ce_tests::fill3< Object >( s1 );

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();

   auto p1 = std::make_tuple( &idx1 );
   auto p2 = std::make_tuple( &idx2 );
   auto p3 = std::make_tuple( &s1 );

   using Iterator = ce::concatenation_iterator< Object, Cmp >;

   Iterator it( Cmp(), p1, p2, p3 );
   Iterator it_end = Iterator::create_end( Cmp(), p1, p2, p3 );

   auto sorted_it = sorted.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *sorted_it ) );

      ++it;
      ++sorted_it;
   }

}

template< typename Call1, typename Call2, typename Collection, typename ID_Index, typename Index, typename Object, typename Cmp, typename SortedCollection >
void test_with_sub_index( Call1& call1, Call2& call2, const SortedCollection& sorted )
{
   Collection bmic1;
   Collection bmic2;

   BOOST_TEST_MESSAGE( "2 filled sources with different content ( sub-index is active )" );
   call1( bmic1 );
   call2( bmic2 );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();

   auto p1 = std::make_tuple( &idx1, &id_idx1 );
   auto p2 = std::make_tuple( &idx2, &id_idx2 );

   using Iterator = ce::concatenation_iterator_ex< Object, Cmp >;

   Iterator it( Cmp(), p1, p2 );
   Iterator it_end = Iterator::create_end( Cmp(), p1, p2 );

   auto sorted_it = sorted.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *sorted_it ) );

      ++it;
      ++sorted_it;
   }

}

template< typename Collection1, typename Collection2, typename ID_Index, typename Index, typename Object, typename Cmp, typename SortedCollection >
void test_with_sub_index_3_sources( const SortedCollection& sorted )
{
   Collection1 bmic1;
   Collection1 bmic2;
   Collection2 s;
   std::set< size_t > s_helper;

   BOOST_TEST_MESSAGE( "3 filled sources with different content ( sub-index is active )" );
   ce_tests::fill4< Object >( bmic1 );
   ce_tests::fill4a< Object >( bmic2 );
   ce_tests::fill4b< Object >( s );

   ce_tests::fill4b_helper( s_helper );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();

   auto p1 = std::make_tuple( &idx1, &id_idx1 );
   auto p2 = std::make_tuple( &idx2, &id_idx2 );
   auto p3 = std::make_tuple( &s, &s_helper );

   using Iterator = ce::concatenation_iterator_ex< Object, Cmp >;

   Iterator it( Cmp(), p1, p2, p3 );
   Iterator it_end = Iterator::create_end( Cmp(), p1, p2, p3 );

   auto sorted_it = sorted.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *sorted_it ) );

      ++it;
      ++sorted_it;
   }

}

template< typename ID_Index, typename Iterator, typename ReverseIterator, typename CmpIterator, typename CmpReverseIterator >
void benchmark_internal( Iterator begin, Iterator end, CmpIterator cmp_begin, CmpIterator cmp_end,
                         ReverseIterator r_begin, ReverseIterator r_end, CmpReverseIterator r_cmp_begin, CmpReverseIterator r_cmp_end,
                         const ID_Index& id_index
                         )
{
   //start benchmark - classical sorted collection
   uint64_t start_time1 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

   while( cmp_begin != cmp_end )
      ++cmp_begin;

   uint64_t end_time1 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

   std::cout<<"iterator: bmic iterator: "<< end_time1 - start_time1<<" ms"<<std::endl;
   //end benchmark

   //start benchmark - concatenation iterator
   uint64_t start_time2 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

   while( begin != end )
      ++begin;

   uint64_t end_time2 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

   std::cout<<"iterator: concatenation iterator: "<< end_time2 - start_time2<<" ms"<<std::endl;
   std::cout<<"ratio: "<<( static_cast< double >( end_time2 - start_time2 ) )/( end_time1 - start_time1 )<<std::endl;
   //end benchmark

   BOOST_TEST_MESSAGE( "reverse_iterator" );
   //start benchmark - classical sorted collection
   start_time1 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

   while( r_cmp_begin != r_cmp_end )
      ++r_cmp_begin;

   end_time1 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

   std::cout<<"reverse_iterator: bmic iterator: "<< end_time1 - start_time1<<" ms"<<std::endl;
   //end benchmark

   //start benchmark - concatenation iterator
   start_time2 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

   while( r_begin != r_end )
      ++r_begin;

   end_time2 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

   std::cout<<"reverse_iterator: concatenation iterator: "<< end_time2 - start_time2<<" ms"<<std::endl;
   std::cout<<"ratio: "<<( static_cast< double >( end_time2 - start_time2 ) )/( end_time1 - start_time1 )<<std::endl;
   //end benchmark
}

template< bool Is_Another_Source, typename Collection, typename ID_Index, typename Index, typename Object, typename Cmp >
void benchmark_test_2_sources()
{
   Collection another;
   Collection bmic1;
   Collection bmic2;
   Collection sorted;

   using t_src = ce_tests::t_input_data;

   const int32_t ratio = 10;
   const int32_t size1 = 1000000;
   const int32_t size2 = size1/ratio;

   std::vector< t_src > v1 = 
   {
      // Example:
      // t_src(
      //       3      /*_length*/,
      //       true   /*bool incremented1*/,    "a"   /*std::string c1*/,    0   /*size_t idx1*/,
      //       true   /*bool incremented2*/,    "b"   /*std::string c2*/,    0   /*size_t idx2*/
      //       )

      t_src( size1, true, "a", 0, true, "b", 0 )
   };
   std::vector< t_src > v2 = 
   {
      t_src( size2, true, "x", 0, true, "y", 0 )
   };

   size_t total_size = size1 + size2;
   int64_t id_cnt = 0;
   int64_t id_cnt_copy = id_cnt;

   BOOST_TEST_MESSAGE( "benchmark test with 2 sources - ( sub-index is active )" );

   //Making sorted collection
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v1 ) );
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v2 ) );
   size_t sorted_size = sorted.size();
   //Making sorted collection

   BOOST_REQUIRE( total_size == sorted_size );

   ce_tests::fill_with_many_objects< Object >( id_cnt, bmic1, v1 );
   ce_tests::fill_with_many_objects< Object >( id_cnt, bmic2, v2 );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();
   const auto& idx_another = another.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx_sorted = sorted.template get< Index >();

   auto p1 = std::make_tuple( &idx1, Is_Another_Source?(&idx_another):(&id_idx1) );
   auto p2 = std::make_tuple( &idx2, Is_Another_Source?(&idx_another):(&id_idx2) );

   using Iterator = ce::concatenation_iterator_ex< Object, Cmp >;
   using ReverseIterator = ce::concatenation_reverse_iterator_ex< Object, Cmp >;

   Iterator it( Cmp(), p1, p2 );
   Iterator it_end = Iterator::create_end( Cmp(), p1, p2 );

   auto sorted_it = idx_sorted.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *sorted_it ) );

      ++it;
      ++sorted_it;
   }

   BOOST_TEST_MESSAGE( "iterator" );
   std::cout<<std::endl<<"***2 sources***"<<std::endl;

   Iterator _begin( Cmp(), p1, p2 );
   Iterator _end = Iterator::create_end( Cmp(), p1, p2 );
   auto cmp_begin = idx_sorted.begin();
   auto cmp_end = idx_sorted.end();

   ReverseIterator _r_begin( Cmp(), p1, p2 );
   ReverseIterator _r_end = ReverseIterator::create_end( Cmp(), p1, p2 );
   auto r_cmp_begin = idx_sorted.rbegin();
   auto r_cmp_end = idx_sorted.rend();

   benchmark_internal( _begin, _end, cmp_begin, cmp_end, _r_begin, _r_end, r_cmp_begin, r_cmp_end,
                        sorted.template get< ID_Index >() );
}

template< bool Is_Another_Source, typename Collection, typename ID_Index, typename Index, typename Object, typename Cmp >
void benchmark_test_3_sources()
{
   Collection another;
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;
   Collection sorted;

   using t_src = ce_tests::t_input_data;

   const int32_t ratio = 10;
   const int32_t size1 = 1000000;
   const int32_t size2 = size1/ratio;
   const int32_t size3 = size1/ratio;

   std::vector< t_src > v1 = 
   {
      // Example:
      // t_src(
      //       3      /*_length*/,
      //       true   /*bool incremented1*/,    "a"   /*std::string c1*/,    0   /*size_t idx1*/,
      //       true   /*bool incremented2*/,    "b"   /*std::string c2*/,    0   /*size_t idx2*/
      //       )

      t_src( size1, true, "a", 0, true, "b", 0 )
   };
   std::vector< t_src > v2 = 
   {
      t_src( size2, true, "x", 0, true, "y", 0 )
   };
   std::vector< t_src > v3 = 
   {
      t_src( size3, true, "p", 0, true, "q", 0 )
   };

   size_t total_size = size1 + size2 + size3;
   int64_t id_cnt = 0;
   int64_t id_cnt_copy = id_cnt;

   BOOST_TEST_MESSAGE( "benchmark test with 3 sources - ( sub-index is active )" );

   //Making sorted collection
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v1 ) );
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v2 ) );
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v3 ) );
   size_t sorted_size = sorted.size();
   //Making sorted collection

   BOOST_REQUIRE( total_size == sorted_size );

   ce_tests::fill_with_many_objects< Object >( id_cnt, bmic1, v1 );
   ce_tests::fill_with_many_objects< Object >( id_cnt, bmic2, v2 );
   ce_tests::fill_with_many_objects< Object >( id_cnt, bmic2, v3 );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();
   const auto& id_idx3 = bmic2.template get< ID_Index >();
   const auto& idx_another = another.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic2.template get< Index >();
   const auto& idx_sorted = sorted.template get< Index >();

   auto p1 = std::make_tuple( &idx1, Is_Another_Source?(&idx_another):(&id_idx1) );
   auto p2 = std::make_tuple( &idx2, Is_Another_Source?(&idx_another):(&id_idx2) );
   auto p3 = std::make_tuple( &idx3, Is_Another_Source?(&idx_another):(&id_idx3) );

   using Iterator = ce::concatenation_iterator_ex< Object, Cmp >;
   using ReverseIterator = ce::concatenation_reverse_iterator_ex< Object, Cmp >;

   Iterator it( Cmp(), p1, p2, p3 );
   Iterator it_end = Iterator::create_end( Cmp(), p1, p2, p3 );

   auto sorted_it = idx_sorted.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *sorted_it ) );

      ++it;
      ++sorted_it;
   }

   BOOST_TEST_MESSAGE( "iterator" );
   std::cout<<std::endl<<"***3 sources***"<<std::endl;

   Iterator _begin( Cmp(), p1, p2, p3 );
   Iterator _end = Iterator::create_end( Cmp(), p1, p2, p3 );
   auto cmp_begin = idx_sorted.begin();
   auto cmp_end = idx_sorted.end();

   ReverseIterator _r_begin( Cmp(), p1, p2, p3 );
   ReverseIterator _r_end = ReverseIterator::create_end( Cmp(), p1, p2, p3 );
   auto r_cmp_begin = idx_sorted.rbegin();
   auto r_cmp_end = idx_sorted.rend();

   benchmark_internal( _begin, _end, cmp_begin, cmp_end, _r_begin, _r_end, r_cmp_begin, r_cmp_end,
                        sorted.template get< ID_Index >() );
}

template< typename Collection, typename ID_Index, typename Index, typename Object, typename Cmp >
void test_with_sub_index_2_sources_many_objects_without_id_repeat()
{
   Collection bmic1;
   Collection bmic2;

   using t_src = ce_tests::t_input_data;

   std::vector< t_src > v1 = 
   {
      /*
         size_t _length
         bool incremented1, std::string c1, size_t idx1
         bool incremented2, std::string c2, size_t idx2
      */
      t_src( 3, true,  "a", 0, true,  "b", 0 ),
      t_src( 5,  false, "c", 1, true,  "d", 10 ),
      t_src( 4,  true,  "e", 2, false, "b", 2 )
   };
   std::vector< t_src > v2 = 
   {
      t_src( 3, true,  "a", 10, true, "b", 10 ),
      t_src( 5,  false, "c", 1, true,  "d", 15 ),
      t_src( 4,  true,  "e", 9, false, "b", 2 ),

      t_src( 100, true,  "b", 1000, true, "a", 100 ),
      t_src( 100,  false, "d", 1000, true,  "c", 150 ),
      t_src( 100,  true,  "b", 1000, false, "e", 200 ),
      t_src( 1000,  true,  "x1000-start", 1000, false, "x1000-end", 200 ),
   };

   size_t total_size = (3+5+4)*2 + 3*100 + 1000;
   int64_t id_cnt = 0;
   int64_t id_cnt_copy = id_cnt;

   BOOST_TEST_MESSAGE( "2 filled sources with different content - many objects without key repeating ( sub-index is active )" );

   //Making sorted collection
   using t_sorted = std::set< Object, Cmp >;
   t_sorted sorted;
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v1 ) );
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v2 ) );
   size_t sorted_size = sorted.size();
   //Making sorted collection

   BOOST_REQUIRE( total_size == sorted_size );

   ce_tests::fill_with_many_objects< Object >( id_cnt, bmic1, v1 );
   ce_tests::fill_with_many_objects< Object >( id_cnt, bmic2, v2 );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();

   auto p1 = std::make_tuple( &idx1, &id_idx1 );
   auto p2 = std::make_tuple( &idx2, &id_idx2 );

   using Iterator = ce::concatenation_iterator_ex< Object, Cmp >;

   Iterator it( Cmp(), p1, p2 );
   Iterator it_end = Iterator::create_end( Cmp(), p1, p2 );

   auto sorted_it = sorted.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *sorted_it ) );

      ++it;
      ++sorted_it;
   }
}

template< typename Collection, typename ID_Index, typename Index, typename Object, typename Cmp >
void test_with_sub_index_2_sources_many_objects_with_id_repeat()
{
   Collection bmic1;
   Collection bmic2;

   using t_src = ce_tests::t_input_data;

   std::vector< t_src > v1 = 
   {
      /*
         size_t _length
         bool incremented1, std::string c1, size_t idx1
         bool incremented2, std::string c2, size_t idx2
      */
      t_src( 3, true,  "a", 0, true,  "b", 0 ),
      t_src( 5,  false, "c", 1, true,  "d", 10 ),
      t_src( 4,  true,  "e", 2, false, "b", 2 ),
      t_src( 100, true,  "b", 1000, true, "a", 100 ),
      t_src( 100,  false, "d", 1000, true,  "c", 150 ),
      t_src( 100,  true,  "b", 1000, false, "e", 200 ),
      t_src( 1000,  true,  "x1000-start", 1000, false, "x1000-end", 200 )
   };
   std::vector< t_src > v2 = 
   {
      t_src( 3, true,  ".a", 0, true,  ".b", 0 ),
      t_src( 5,  false, ".c", 1, true,  ".d", 10 ),
      t_src( 4,  true,  ".e", 2, false, ".b", 2 ),
      t_src( 100, true,  ".b", 1000, true, ".a", 100 ),
      t_src( 100,  false, ".d", 1000, true,  ".c", 150 ),
      t_src( 100,  true,  ".b", 1000, false, ".e", 200 ),
      t_src( 1000,  true,  ".x1000-start", 1000, false, ".x1000-end", 200 )
   };

   size_t total_size = 3+5+4 + 3*100 + 1000;

   int64_t id_cnt = 0;

   int64_t id_cnt_copy = id_cnt;
   int64_t id_cnt_copy1 = id_cnt;
   int64_t id_cnt_copy2 = id_cnt;

   BOOST_TEST_MESSAGE( "2 filled sources with different content - many objects with key repeating ( sub-index is active )" );

   //Making sorted collection
   using t_sorted = std::set< Object, Cmp >;
   t_sorted sorted;
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v2 ) );
   size_t sorted_size = sorted.size();
   //Making sorted collection

   BOOST_REQUIRE( total_size == sorted_size );

   ce_tests::fill_with_many_objects< Object >( id_cnt_copy1, bmic1, v1 );
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy2, bmic2, v2 );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();

   auto p1 = std::make_tuple( &idx1, &id_idx1 );
   auto p2 = std::make_tuple( &idx2, &id_idx2 );

   using Iterator = ce::concatenation_iterator_ex< Object, Cmp >;

   Iterator it( Cmp(), p1, p2 );
   Iterator it_end = Iterator::create_end( Cmp(), p1, p2 );

   auto sorted_it = sorted.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *sorted_it ) );

      ++it;
      ++sorted_it;
   }
}

template< typename Collection, typename ID_Index, typename Index, typename Object, typename Cmp >
void test_with_sub_index_3_sources_many_objects_with_id_repeat()
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   using t_src = ce_tests::t_input_data;

   std::vector< t_src > v1 = 
   {
      /*
         size_t _length
         bool incremented1, std::string c1, size_t idx1
         bool incremented2, std::string c2, size_t idx2
      */
      t_src( 3, true,  "a", 0, true,  "b", 0 ),
      t_src( 5,  false, "c", 1, true,  "d", 10 ),
      t_src( 4,  true,  "e", 2, false, "b", 2 ),
      t_src( 100, true,  "b", 1000, true, "a", 100 ),
      t_src( 100,  false, "d", 1000, true,  "c", 150 ),
      t_src( 100,  true,  "b", 1000, false, "e", 200 ),
      t_src( 1000,  true,  "x1000-start", 1000, false, "x1000-end", 200 )
   };
   std::vector< t_src > v2 = 
   {
      t_src( 3, true,  ".a", 0, true,  ".b", 0 ),
      t_src( 5,  false, ".c", 1, true,  ".d", 10 ),
      t_src( 4,  true,  ".e", 2, false, ".b", 2 ),
      t_src( 100, true,  ".b", 1000, true, ".a", 100 ),
      t_src( 100,  false, ".d", 1000, true,  ".c", 150 ),
      t_src( 100,  true,  ".b", 1000, false, ".e", 200 ),
      t_src( 1000,  true,  ".x1000-start", 1000, false, ".x1000-end", 200 )
   };
   std::vector< t_src > v3 = 
   {
      t_src( 3, true,  "!.a", 0, true,  "!.b", 0 ),
      t_src( 5,  false, "!.c", 1, true,  "!.d", 10 ),
      t_src( 4,  true,  "!.e", 2, false, "!.b", 2 ),
      t_src( 100, true,  "!.b", 1000, true, "!.a", 100 ),
      t_src( 100,  false, "!.d", 1000, true,  "!.c", 150 ),
      t_src( 100,  true,  "!.b", 1000, false, "!.e", 200 ),
      t_src( 1000,  true,  "!.x1000-start", 1000, false, "!.x1000-end", 200 )
   };

   size_t total_size = 3+5+4 + 3*100 + 1000;

   int64_t id_cnt = 0;

   int64_t id_cnt_copy = id_cnt;
   int64_t id_cnt_copy1 = id_cnt;
   int64_t id_cnt_copy2 = id_cnt;
   int64_t id_cnt_copy3 = id_cnt;

   BOOST_TEST_MESSAGE( "3 filled sources with different content - many objects with key repeating ( sub-index is active )" );

   //Making sorted collection
   using t_sorted = std::set< Object, Cmp >;
   t_sorted sorted;
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy, sorted, std::vector< t_src >( v3 ) );
   size_t sorted_size = sorted.size();
   //Making sorted collection

   BOOST_REQUIRE( total_size == sorted_size );

   ce_tests::fill_with_many_objects< Object >( id_cnt_copy1, bmic1, v1 );
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy2, bmic2, v2 );
   ce_tests::fill_with_many_objects< Object >( id_cnt_copy3, bmic3, v3 );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();
   const auto& id_idx3 = bmic2.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   auto p1 = std::make_tuple( &idx1, &id_idx1 );
   auto p2 = std::make_tuple( &idx2, &id_idx2 );
   auto p3 = std::make_tuple( &idx3, &id_idx3 );

   using Iterator = ce::concatenation_iterator_ex< Object, Cmp >;

   Iterator it( Cmp(), p1, p2, p3 );
   Iterator it_end = Iterator::create_end( Cmp(), p1, p2, p3 );

   auto sorted_it = sorted.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *sorted_it ) );

      ++it;
      ++sorted_it;
   }
}

template< typename Iterator, typename ReverseIterator, typename Collection, typename Object, typename Index, typename Cmp, typename Filler >
void different_test( Filler&& filler )
{
   Collection bmic1;

   BOOST_TEST_MESSAGE( "1 source - different tests" );
   filler( bmic1 );

   const auto& idx1 = bmic1.template get< Index >();

   auto p1 = std::make_tuple( &idx1 );

   {
      Iterator it( Cmp(), p1 );
      Iterator it_begin( it );
      Iterator it_end = Iterator::create_end( Cmp(), p1 );

      ReverseIterator it_r( Cmp(), p1 );
      ReverseIterator it_r_begin( it_r );
      ReverseIterator it_r_end = Iterator::create_end( Cmp(), p1 );

      auto it_comparer = idx1.begin();
      auto it_r_comparer = idx1.rbegin();

      while( it != it_end )
      {
         BOOST_REQUIRE( *it == *it_comparer );
         BOOST_REQUIRE( *it_r == *it_r_comparer );

         it = std::next( it, 1 );
         it_r = std::next( it_r, 1 );
         ++it_comparer;
         ++it_r_comparer;
      }

      for( size_t i = 0; i<idx1.size(); ++i )
      {
         it = std::prev( it, 1 );
         it_r = std::prev( it_r, 1 );
         it_comparer = std::prev( it_comparer, 1 );
         it_r_comparer = std::prev( it_r_comparer, 1 );

         BOOST_REQUIRE( *it == *it_comparer );
         BOOST_REQUIRE( *it_r == *it_r_comparer );
      }

      it = it_begin;
      it_r = it_r_begin;
      it_comparer = idx1.begin();
      it_r_comparer = idx1.rbegin();

      it = std::next( it, 3 );
      it_r = std::next( it_r, 3 );
      it_comparer = std::next( it_comparer, 3 );
      it_r_comparer = std::next( it_r_comparer, 3 );
      BOOST_REQUIRE( *it == *it_comparer );
      BOOST_REQUIRE( *it_r == *it_r_comparer );

      it = std::prev( it, 2 );
      it_r = std::prev( it_r, 2 );
      it_comparer = std::prev( it_comparer, 2 );
      it_r_comparer = std::prev( it_r_comparer, 2 );
      BOOST_REQUIRE( *it == *it_comparer );
      BOOST_REQUIRE( *it_r == *it_r_comparer );

      it = std::prev( it, 1 );
      it_r = std::prev( it_r, 1 );
      it_comparer = std::prev( it_comparer, 1 );
      it_r_comparer = std::prev( it_r_comparer, 1 );
      BOOST_REQUIRE( *it == *it_comparer );
      BOOST_REQUIRE( *it_r == *it_r_comparer );
   }

   {
      ReverseIterator it_r_end = ReverseIterator::create_end( Cmp(), p1 );
      it_r_end = std::prev( it_r_end, 1 );
      auto it_comparer = idx1.begin();

      while( it_comparer != idx1.end() )
      {
         BOOST_REQUIRE( *it_r_end == *it_comparer );
         --it_r_end;
         it_comparer++;
      }
   }

   {
      ReverseIterator it_r_end = ReverseIterator::create_end( Cmp(), p1 );

      ReverseIterator it_r( Iterator::create_end( Cmp(), p1 ) );
      decltype( idx1.rbegin() ) it_r_comparer( idx1.end() );

      BOOST_REQUIRE( it_r == ReverseIterator( Cmp(), p1 ) );
      BOOST_REQUIRE( *it_r == *it_r_comparer );

      while( it_r_comparer != idx1.rend() )
      {
         BOOST_REQUIRE( *it_r == *it_r_comparer );

         it_r = std::next( it_r, 1 );
         it_r_comparer = std::next( it_r_comparer, 1 );
      }
   }

   {
      auto it_comparer = idx1.end();
      it_comparer = std::prev( it_comparer, 1 );

      ReverseIterator it_r( Cmp(), p1 );
      std::reverse_iterator< Iterator > std_it_r ( it_r );
      Iterator it = std_it_r.base();
      BOOST_REQUIRE( *it == *it_comparer );

      it_comparer = std::prev( it_comparer, 1 );
      it = std::prev( it, 1 );
      BOOST_REQUIRE( *it == *it_comparer );
   }

   {
      auto it_comparer = idx1.end();
      Iterator it = Iterator::create_end( Cmp(), p1 );
      --it;
      --it_comparer;
      BOOST_REQUIRE( *it == *it_comparer );
   }

   {
      auto it_comparer = idx1.begin();

      ReverseIterator it_r = ReverseIterator::create_end( Cmp(), p1 );
      it_r--;
      std::reverse_iterator< Iterator > std_it_r ( it_r );
      Iterator it = std_it_r.base();
      BOOST_REQUIRE( *it == *it_comparer );
   }
}

template< typename Iterator, typename ReverseIterator, typename Collection, typename Object, typename ID_Index, typename Index, typename Cmp, typename Filler1, typename Filler2, typename Filler3, typename SortedFiller >
void different_test_sub_index( Filler1& filler1, Filler2& filler2, Filler3& filler3, SortedFiller& sorted_filler )
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   BOOST_TEST_MESSAGE( "3 sources - different tests ( sub-index is active )" );
   filler1( bmic1 );
   filler2( bmic2 );
   filler3( bmic3 );
   sorted_filler( comparer );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();
   const auto& id_idx3 = bmic3.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   const auto& comparer_idx = comparer.template get< Index >();
   auto it_comparer = comparer_idx.begin();
   auto it_r_comparer = comparer_idx.rbegin();

   {
      Iterator it( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );
      Iterator it_begin( it );
      Iterator it_end = Iterator::create_end( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );

      ReverseIterator it_r( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );
      ReverseIterator it_r_begin( it_r );
      ReverseIterator it_r_end = ReverseIterator::create_end( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );

      while( it != it_end )
      {
         BOOST_REQUIRE( *it == *it_comparer );
         BOOST_REQUIRE( *it_r == *it_r_comparer );

         it = std::next( it, 1 );
         it_r = std::next( it_r, 1 );
         ++it_comparer;
         ++it_r_comparer;
      }

      for( size_t i = 0; i<comparer_idx.size(); ++i )
      {
         it = std::prev( it, 1 );
         it_r = std::prev( it_r, 1 );
         it_comparer = std::prev( it_comparer, 1 );
         it_r_comparer = std::prev( it_r_comparer, 1 );

         BOOST_REQUIRE( *it == *it_comparer );
         BOOST_REQUIRE( *it_r == *it_r_comparer );
      }

      it = it_begin;
      it_r = it_r_begin;
      it_comparer = comparer_idx.begin();
      it_r_comparer = comparer_idx.rbegin();

      it = std::next( it, 3 );
      it_r = std::next( it_r, 3 );
      it_comparer = std::next( it_comparer, 3 );
      it_r_comparer = std::next( it_r_comparer, 3 );
      BOOST_REQUIRE( *it == *it_comparer );
      BOOST_REQUIRE( *it_r == *it_r_comparer );

      it = std::prev( it, 2 );
      it_r = std::prev( it_r, 2 );
      it_comparer = std::prev( it_comparer, 2 );
      it_r_comparer = std::prev( it_r_comparer, 2 );
      BOOST_REQUIRE( *it == *it_comparer );
      BOOST_REQUIRE( *it_r == *it_r_comparer );

      it = std::prev( it, 1 );
      it_r = std::prev( it_r, 1 );
      it_comparer = std::prev( it_comparer, 1 );
      it_r_comparer = std::prev( it_r_comparer, 1 );
      BOOST_REQUIRE( *it == *it_comparer );
      BOOST_REQUIRE( *it_r == *it_r_comparer );
   }

   {
      ReverseIterator it_r_end = ReverseIterator::create_end( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );

      ReverseIterator it_r( Iterator::create_end( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         ) );
      decltype( comparer_idx.rbegin() ) it_r_comparer( comparer_idx.end() );

      BOOST_REQUIRE( it_r == ReverseIterator( Cmp(), 
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 ) ) );
      BOOST_REQUIRE( *it_r == *it_r_comparer );

      while( it_r != it_r_end )
      {
         BOOST_REQUIRE( *it_r == *it_r_comparer );

         it_r = std::next( it_r, 1 );
         it_r_comparer = std::next( it_r_comparer, 1 );
      }
   }
}

template< typename ReverseIterator, typename Collection, typename Object, typename Index, typename Cmp, typename Filler >
void inc_dec_basic_reverse_test( Filler&& filler )
{
   Collection bmic1;

   BOOST_TEST_MESSAGE( "1 source - reverse_iterator: operations: '++'" );
   filler( bmic1 );

   const auto& idx1 = bmic1.template get< Index >();

   auto p1 = std::make_tuple( &idx1 );
   ReverseIterator it_r( Cmp(), p1 );
   ReverseIterator it_end_r = ReverseIterator::create_end( Cmp(), p1 );

   auto it_comparer_r = idx1.rbegin();
   auto it01_r( it_r );

   while( it_r != it_end_r )
   {
      BOOST_REQUIRE( *it_r == *it_comparer_r );
      ++it_r;
      ++it_comparer_r;
   }

   BOOST_REQUIRE( it_r == it_end_r );

   it_comparer_r = idx1.rbegin();
   while( it_comparer_r != idx1.rend() )
   {
      BOOST_REQUIRE( *it01_r == *it_comparer_r );
      ++it01_r;
      ++it_comparer_r;
   }
}

template< typename Iterator, typename ReverseIterator, typename Collection, typename Object, typename Index, typename Cmp, typename Filler >
void inc_dec_basic_1_source_test( Filler&& filler )
{
   Collection bmic1;

   BOOST_TEST_MESSAGE( "1 source - operations: '++' and '--'" );
   filler( bmic1 );

   const auto& idx1 = bmic1.template get< Index >();

   auto p1 = std::make_tuple( &idx1 );
   Iterator it( Cmp(), p1 );
   ReverseIterator it_r( Cmp(), p1 );
   ReverseIterator it_end_r = ReverseIterator::create_end( Cmp(), p1 );

   auto it_comparer = idx1.begin();
   auto it_comparer_r = idx1.rbegin();

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_r;
   ++it_comparer;
   ++it_comparer_r;

   --it;
   --it_r;
   --it_comparer;
   --it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_r;
   ++it_comparer;
   ++it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_r;
   ++it_comparer;
   ++it_comparer_r;

   --it;
   --it_r;
   --it_comparer;
   --it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_r;
   ++it_comparer;
   ++it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_r;
   ++it_comparer;
   ++it_comparer_r;

   --it;
   --it_r;
   --it_comparer;
   --it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_r;
   ++it_comparer;
   ++it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_r;
   ++it_comparer;
   ++it_comparer_r;

   BOOST_REQUIRE( it_r == it_end_r );

   --it;
   --it_r;
   --it_comparer;
   --it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   --it;
   --it_r;
   --it_comparer;
   --it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   --it;
   --it_r;
   --it_comparer;
   --it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );

   --it;
   --it_r;
   --it_comparer;
   --it_comparer_r;

   BOOST_REQUIRE( *it_r == *it_comparer_r );
   BOOST_REQUIRE( *it == *it_comparer );
}

template< typename Collection, typename Object, typename Index, typename Cmp, typename Filler1, typename Filler2, typename Filler3, typename SortedFiller >
void inc_dec_basic_3_sources_test( Filler1& filler1, Filler2& filler2, Filler3& filler3, SortedFiller& sorted_filler )
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   BOOST_TEST_MESSAGE( "3 sources - operations: '++' and '--'" );
   filler1( bmic1 );
   filler2( bmic2 );
   filler3( bmic3 );
   sorted_filler( comparer );

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   ce::concatenation_iterator< Object, Cmp > it( Cmp(),
                                                   std::make_tuple( &idx1 ),
                                                   std::make_tuple( &idx2 ),
                                                   std::make_tuple( &idx3 )
                                                );

   auto it_comparer = comparer.begin();

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it;
   --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it;
   --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it;
   --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;

   --it;
   --it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;

   --it;
   --it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it;
   ++it_comparer;

   --it;
   --it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it;
   --it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it;
   --it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it;
   --it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it;
   --it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it;
   --it_comparer;

   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
}

template< typename Collection, typename Object, typename Index, typename Cmp,
typename Filler1, typename Filler2, typename Filler3, typename Filler4, typename Filler5, typename Filler6, typename SortedFiller >
void inc_dec_basic_6_sources_test( Filler1& f1, Filler2& f2, Filler3& f3, Filler4& f4, Filler5& f5, Filler6& f6, SortedFiller& sorted_filler )
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;
   Collection bmic4;
   Collection bmic5;
   Collection bmic6;

   Collection comparer;

   BOOST_TEST_MESSAGE( "6 sources - operations: '++' and '--'" );
   f1( bmic1 );
   f2( bmic2 );
   f3( bmic3 );
   f4( bmic4 );
   f5( bmic5 );
   f6( bmic6 );
   sorted_filler( comparer );

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();
   const auto& idx4 = bmic4.template get< Index >();
   const auto& idx5 = bmic5.template get< Index >();
   const auto& idx6 = bmic6.template get< Index >();

   ce::concatenation_iterator< Object, Cmp > it( Cmp(),
                                                   std::make_tuple( &idx1 ),
                                                   std::make_tuple( &idx2 ),
                                                   std::make_tuple( &idx3 ),
                                                   std::make_tuple( &idx4 ),
                                                   std::make_tuple( &idx5 ),
                                                   std::make_tuple( &idx6 )
                                                );

   auto it_comparer = comparer.begin();
   size_t size = comparer.size();

   for( size_t i = 0 ; i < size; ++i )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      ++it;
      ++it_comparer;
   }

   --it;
   --it_comparer;

   for( size_t i = 1 ; i < size; ++i )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      --it;
      --it_comparer;
   }

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   ++it; ++it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );

   --it; --it_comparer;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
}

template< typename Iterator, typename ReverseIterator, typename Collection, typename Object, typename ID_Index, typename Index, typename Cmp,
typename Filler1, typename Filler2, typename Filler3, typename SortedFiller >
void inc_dec_basic_3_sources_sub_index_test( Filler1& f1, Filler2& f2, Filler3& f3, SortedFiller& sorted_filler )
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   BOOST_TEST_MESSAGE( "3 sources - operations: '++' and '--' ( sub-index is active )" );
   f1( bmic1 );
   f2( bmic2 );
   f3( bmic3 );
   sorted_filler( comparer );

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();
   const auto& id_idx3 = bmic3.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   Iterator it( Cmp(),
                  std::make_tuple( &idx1, &id_idx1 ),
                  std::make_tuple( &idx2, &id_idx2 ),
                  std::make_tuple( &idx3, &id_idx3 )
               );

   ReverseIterator it_r( Cmp(),
                           std::make_tuple( &idx1, &id_idx1 ),
                           std::make_tuple( &idx2, &id_idx2 ),
                           std::make_tuple( &idx3, &id_idx3 )
                        );

   const auto& comparer_idx = comparer.template get< Index >();
   auto it_comparer = comparer_idx.begin();
   auto it_comparer_r = comparer_idx.rbegin();

   size_t size = comparer_idx.size();

   for( size_t i = 0 ; i < size; ++i )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );
      ++it;
      ++it_comparer;
      ++it_r;
      ++it_comparer_r;
   }

   for( size_t i = 0 ; i < size; ++i )
   {
      --it;
      --it_comparer;
      --it_r;
      --it_comparer_r;
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );
   }

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   ++it; ++it_comparer;
   ++it_r; ++it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );

   --it; --it_comparer;
   --it_r; --it_comparer_r;
   BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
   BOOST_REQUIRE( ( *it_r ) == ( *it_comparer_r ) );
}

template< typename Iterator, typename Collection, typename Object, typename Index, typename Cmp, typename Filler1, typename Filler2, typename Filler3, typename SortedFiller >
void comparision_assignment_test( Filler1& filler1, Filler2& filler2, Filler3& filler3, SortedFiller& sorted_filler )
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   BOOST_TEST_MESSAGE( "3 sources - assignments, comparisions" );
   filler1( bmic1 );
   filler2( bmic2 );
   filler3( bmic3 );
   sorted_filler( comparer );

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   using _t = Iterator;

   _t _it( Cmp(),
            std::make_tuple( &idx1 ),
            std::make_tuple( &idx2 ),
            std::make_tuple( &idx3 )
         );

   _t it_end = _t::create_end( Cmp(),
            std::make_tuple( &idx1 ),
            std::make_tuple( &idx2 ),
            std::make_tuple( &idx3 )
         );

   _t it_begin( _it );
   _t it( _it );

   BOOST_REQUIRE( it == _it );
   BOOST_REQUIRE( it == it_begin );
   BOOST_REQUIRE( _it == it_begin );
   BOOST_REQUIRE( it != it_end );

   ++it;

   BOOST_REQUIRE( _it == it_begin );

   --it;
   BOOST_REQUIRE( it == it_begin );

   ++_it;
   ++_it;
   --_it;
   --_it;
   BOOST_REQUIRE( _it == it_begin );
   BOOST_REQUIRE( _it == it );

   auto it_comparer = comparer.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      ++it;
      ++it_comparer;
   }

   it = it_begin;
   it_comparer = comparer.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      ++it;
      ++it_comparer;
   }

   it = it_end;
   BOOST_REQUIRE( it == it_end );

   BOOST_REQUIRE( _it == it_begin );
   ++_it;
   ++_it;
   it_comparer = comparer.begin();
   ++it_comparer;
   ++it_comparer;
   BOOST_REQUIRE( ( *_it ) == ( *it_comparer ) );
   it = _it;
   BOOST_REQUIRE( it == _it );
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      ++it;
      ++it_comparer;
   }
}

template< typename Iterator, typename Collection, typename Object, typename ID_Index, typename Index, typename Cmp, typename Filler1, typename Filler2, typename Filler3, typename SortedFiller >
void comparision_assignment_sub_index_test( Filler1& filler1, Filler2& filler2, Filler3& filler3, SortedFiller& sorted_filler )
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   BOOST_TEST_MESSAGE( "3 sources - assignments, comparisions ( sub-index is active )" );
   filler1( bmic1 );
   filler2( bmic2 );
   filler3( bmic3 );
   sorted_filler( comparer );

   using _t = Iterator;

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();
   const auto& id_idx3 = bmic3.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   _t _it( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );

   _t it_end = _t::create_end( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );

   _t it_begin( _it );
   _t it( _it );

   BOOST_REQUIRE( it == _it );
   BOOST_REQUIRE( it == it_begin );
   BOOST_REQUIRE( _it == it_begin );
   BOOST_REQUIRE( it != it_end );

   ++it;

   BOOST_REQUIRE( _it == it_begin );

   --it;
   BOOST_REQUIRE( it == it_begin );

   ++_it;
   ++_it;
   --_it;
   --_it;
   BOOST_REQUIRE( _it == it_begin );
   BOOST_REQUIRE( _it == it );

   const auto& comparer_idx = comparer.template get< Index >();
   auto it_comparer = comparer_idx.begin();

   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      ++it;
      ++it_comparer;
   }

   it = it_begin;
   it_comparer = comparer_idx.begin();
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      ++it;
      ++it_comparer;
   }

   it = it_end;
   BOOST_REQUIRE( it == it_end );

   BOOST_REQUIRE( _it == it_begin );
   ++_it;
   ++_it;
   it_comparer = comparer_idx.begin();
   ++it_comparer;
   ++it_comparer;
   BOOST_REQUIRE( ( *_it ) == ( *it_comparer ) );
   it = _it;
   BOOST_REQUIRE( it == _it );
   while( it != it_end )
   {
      BOOST_REQUIRE( ( *it ) == ( *it_comparer ) );
      ++it;
      ++it_comparer;
   }
}

template< typename Iterator, typename Collection, typename Object, typename ID_Index, typename Index, typename Cmp, typename Filler1, typename Filler2, typename Filler3, typename SortedFiller >
void misc_operations_sub_index_test( Filler1& filler1, Filler2& filler2, Filler3& filler3, SortedFiller& sorted_filler )
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   BOOST_TEST_MESSAGE( "3 sources - miscellaneous operations ( sub-index is active )" );
   filler1( bmic1 );
   filler2( bmic2 );
   filler3( bmic3 );
   sorted_filler( comparer );

   using _t = Iterator;

   const auto& id_idx1 = bmic1.template get< ID_Index >();
   const auto& id_idx2 = bmic2.template get< ID_Index >();
   const auto& id_idx3 = bmic3.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   _t begin( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );

   _t end = _t::create_end( Cmp(),
            std::make_tuple( &idx1, &id_idx1 ),
            std::make_tuple( &idx2, &id_idx2 ),
            std::make_tuple( &idx3, &id_idx3 )
         );

   const auto& comparer_idx = comparer.template get< Index >();
   auto comp = comparer_idx.begin();

   _t it_01( begin );
   BOOST_REQUIRE( it_01 == begin );
  
   it_01++;
   ++comp;
   BOOST_REQUIRE( *it_01 == *comp );

   it_01--;
   --comp;
   BOOST_REQUIRE( *it_01 == *comp );
   BOOST_REQUIRE( it_01 == begin );

   auto it_02( it_01++ );
   BOOST_REQUIRE( *it_02 == *comp );
   BOOST_REQUIRE( it_02 == begin );

   it_01--;
   BOOST_REQUIRE( it_02 == it_01 );

   it_01++;
   it_01++;
   it_01++;
   comp = std::next( comp, 3 );
   BOOST_REQUIRE( *it_01 == *comp );

   auto it_03( it_01-- );
   BOOST_REQUIRE( *it_03 == *comp );
   it_01++;
   BOOST_REQUIRE( it_03 == it_01 );

   auto it_04( it_01 );
   auto it_05( it_01 );

   ++it_04;
   it_05++;
   BOOST_REQUIRE( it_04 == it_05 );
   --it_05;

   auto it_06( it_05-- );
   ++it_05;
   BOOST_REQUIRE( it_06 == it_05 );

   auto it_07( end );
   comp = comparer_idx.end();
   --it_07;
   --comp;
   BOOST_REQUIRE( *it_07 == *comp );

   auto end_02( end );
   auto it_08( end_02-- );
   it_08--;
   BOOST_REQUIRE( *it_08 == *comp );

   auto end_03( end );
   auto it_09( --end_03 );
   comp = comparer_idx.end();
   --comp;
   BOOST_REQUIRE( *it_09 == *comp );

   it_09++;
   BOOST_REQUIRE( it_09 == end );

   comp = comparer_idx.begin();
   it_09 = begin;
   BOOST_REQUIRE( it_09 == begin );
   BOOST_REQUIRE( *it_09 == *comp );

   it_09 = end;
   BOOST_REQUIRE( it_09 == end );

   it_08 = it_09;
   BOOST_REQUIRE( it_08 == end );

   it_08 = it_09 = begin;
   BOOST_REQUIRE( ( it_08 == it_09 ) && ( it_09 == begin ) );
   BOOST_REQUIRE( *it_09 == *comp );

   it_08 = ++it_09;
   BOOST_REQUIRE( it_08 == it_09 );
   ++comp;
   BOOST_REQUIRE( *it_09 == *comp );

   it_08 = it_09++;
   BOOST_REQUIRE( it_08 != it_09 );
   ++comp;
   BOOST_REQUIRE( *it_09 == *comp );

   it_08++;
   --it_08;
   ++it_08;
   it_08--;
   it_08++;
   BOOST_REQUIRE( it_08 == it_09 );
}

template< typename Object, typename Index, typename Callable >
void modify( uint32_t id, Index& idx, Callable&& call )
{
   auto found = idx.find( id );
   BOOST_REQUIRE( found != idx.end() );
   idx.modify( found, call );
}

template< typename Object, typename Collection >
void fill_for_modification_tests( Collection& c1, Collection& c2, Collection& c3, Collection& comparer )
{
   //bmic1
   auto c0_a = []( Object& obj ){ obj.id = 0; obj.val = 5; obj.val2 = 1; obj.val3 = 0; };
   auto c1_a = []( Object& obj ){ obj.id = 1; obj.val = 6; obj.val2 = 2; obj.val3 = 1; };
   auto c2_a = []( Object& obj ){ obj.id = 2; obj.val = 8; obj.val2 = 4; obj.val3 = 2; };
   auto c3_a = []( Object& obj ){ obj.id = 3; obj.val = 9; obj.val2 = 4; obj.val3 = 3; };
   c1.emplace( Object ( c0_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c1_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c2_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c3_a, std::allocator< Object >() ) );

   //bmic2
   auto c0_b = []( Object& obj ){ obj.id = 4; obj.val = 0; obj.val2 = 6; obj.val3 = 10; };
   auto c1_b = []( Object& obj ){ obj.id = 5; obj.val = 5; obj.val2 = 0; obj.val3 = 11; };
   auto c2_b = []( Object& obj ){ obj.id = 2; obj.val = 6; obj.val2 = 3; obj.val3 = 12; };
   auto c3_b = []( Object& obj ){ obj.id = 7; obj.val = 7; obj.val2 = 2; obj.val3 = 13; };
   c2.emplace( Object ( c0_b, std::allocator< Object >() ) );
   c2.emplace( Object ( c1_b, std::allocator< Object >() ) );
   c2.emplace( Object ( c2_b, std::allocator< Object >() ) );
   c2.emplace( Object ( c3_b, std::allocator< Object >() ) );

   //bmic3
   auto c0_c = []( Object& obj ){ obj.id = 5; obj.val = 1; obj.val2 = 6; obj.val3 = 20; };
   auto c1_c = []( Object& obj ){ obj.id = 1; obj.val = 5; obj.val2 = 3; obj.val3 = 21; };
   auto c2_c = []( Object& obj ){ obj.id = 2; obj.val = 6; obj.val2 = 4; obj.val3 = 22; };
   c3.emplace( Object ( c0_c, std::allocator< Object >() ) );
   c3.emplace( Object ( c1_c, std::allocator< Object >() ) );
   c3.emplace( Object ( c2_c, std::allocator< Object >() ) );

   //comparer
   auto c0_ = []( Object& obj ){ obj.id = 4; obj.val = 0; obj.val2 = 6; obj.val3 = 10; };
   auto c1_ = []( Object& obj ){ obj.id = 5; obj.val = 1; obj.val2 = 6; obj.val3 = 20; };
   auto c2_ = []( Object& obj ){ obj.id = 0; obj.val = 5; obj.val2 = 1; obj.val3 = 0; };
   auto c3_ = []( Object& obj ){ obj.id = 1; obj.val = 5; obj.val2 = 3; obj.val3 = 21; };
   auto c4_ = []( Object& obj ){ obj.id = 2; obj.val = 6; obj.val2 = 4; obj.val3 = 22; };
   auto c5_ = []( Object& obj ){ obj.id = 7; obj.val = 7; obj.val2 = 2; obj.val3 = 13; };
   auto c6_ = []( Object& obj ){ obj.id = 3; obj.val = 9; obj.val2 = 4; obj.val3 = 3; };
   comparer.emplace( Object ( c0_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c1_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c2_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c3_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c4_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c5_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c6_, std::allocator< Object >() ) );
}

template< typename Collection, typename Index >
using t_index_type = decltype( std::declval< Collection >().template get< Index >() );

template< typename Collection, typename Index >
using _type_forward = decltype( std::declval< t_index_type< Collection, Index > >().begin() );

template< typename Collection, typename Index >
using _type_reverse = decltype( std::declval< t_index_type< Collection, Index > >().rbegin() );

template< bool Forward, typename Collection, typename Index >
using t_direction = typename std::conditional< Forward, _type_forward< Collection, Index >, _type_reverse< Collection, Index > >::type;

template< typename Collection, typename Index, typename Result >
class helper_iterator
{
   Result get_begin( t_index_type< Collection, Index >& idx );
};

template< typename Collection, typename Index >
struct helper_iterator< Collection, Index, _type_forward< Collection, Index > >
{
   _type_forward< Collection, Index > get_begin( t_index_type< Collection, Index >& idx )
   {
      return idx.begin();
   }
};

template< typename Collection, typename Index >
struct helper_iterator< Collection, Index, _type_reverse< Collection, Index > >
{
   _type_reverse< Collection, Index > get_begin( t_index_type< Collection, Index >& idx )
   {
      return idx.rbegin();
   }
};

template< bool Forward, typename Object, typename CMP, typename ID_Index, typename Index, typename Collection, typename Iterator >
void modification_test_1()
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   fill_for_modification_tests< Object >( bmic1, bmic2, bmic3, comparer );

   auto& id_idx1 = bmic1.template get< ID_Index >();
   auto& id_idx2 = bmic2.template get< ID_Index >();
   auto& id_idx3 = bmic3.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   Iterator it( CMP(),
               std::make_tuple( &idx1, &id_idx1 ),
               std::make_tuple( &idx2, &id_idx2 ),
               std::make_tuple( &idx3, &id_idx3 )
            );

   auto& id_comparer_idx = comparer.template get< ID_Index >();
   auto& comparer_idx = comparer.template get< Index >();
 
   helper_iterator< Collection, Index, t_direction< Forward, Collection, Index > > hi;
   auto it_comparer = hi.get_begin( comparer_idx );

   BOOST_TEST_MESSAGE( "2 elements forward" );

   ++it;
   ++it_comparer;

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 4; val = 0; val2 = 6; val3 = 10;
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 0; val = 5; val2 = 1; val3 = 0;   X
      id = 1; val = 5; val2 = 3; val3 = 21;
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
   */
   BOOST_TEST_MESSAGE( "modifying current element, which is moved back" );
   modify< Object >( 0/*id*/, id_idx1, [&]( Object& obj ){ obj.val = 0; } );
   modify< Object >( 0/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 0; } );
   it.get_modifier().add_modify( 0, 0 );
   /*
      id = 0; val = 0; val2 = 1; val3 = 0;   X
      id = 4; val = 0; val2 = 6; val3 = 10;
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 1; val = 5; val2 = 3; val3 = 21;
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
   */
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );
   /*
      id = 0; val = 0; val2 = 1; val3 = 0;
      id = 4; val = 0; val2 = 6; val3 = 10;
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 1; val = 5; val2 = 3; val3 = 21;  X
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
   */
   BOOST_TEST_MESSAGE( "modifying current element, which is moved forward" );
   modify< Object >( 1/*id*/, id_idx3, [&]( Object& obj ){ obj.val = 8; } );
   modify< Object >( 1/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 8; } );
   it.get_modifier().add_modify( 1, 2 );
   /*
      id = 0; val = 0; val2 = 1; val3 = 0;
      id = 4; val = 0; val2 = 6; val3 = 10;
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 1; val = 8; val2 = 3; val3 = 21;
      id = 3; val = 9; val2 = 4; val3 = 3;   X
   */
   BOOST_REQUIRE( *it == *it_comparer );
   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );
}

template< bool Forward, typename Object, typename CMP, typename ID_Index, typename Index, typename Collection, typename Iterator >
void modification_test_2()
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   fill_for_modification_tests< Object >( bmic1, bmic2, bmic3, comparer );

   auto& id_idx1 = bmic1.template get< ID_Index >();
   auto& id_idx2 = bmic2.template get< ID_Index >();
   auto& id_idx3 = bmic3.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   Iterator it( CMP(),
               std::make_tuple( &idx1, &id_idx1 ),
               std::make_tuple( &idx2, &id_idx2 ),
               std::make_tuple( &idx3, &id_idx3 )
            );
   
   auto& id_comparer_idx = comparer.template get< ID_Index >();
   auto& comparer_idx = comparer.template get< Index >();

   helper_iterator< Collection, Index, t_direction< Forward, Collection, Index > > hi;
   auto it_comparer = hi.get_begin( comparer_idx );

   BOOST_TEST_MESSAGE( "2 elements forward" );
   ++it;
   ++it_comparer;

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 4; val = 0; val2 = 6; val3 = 10;
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 0; val = 5; val2 = 1; val3 = 0;   X
      id = 1; val = 5; val2 = 3; val3 = 21;
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
   */
   BOOST_TEST_MESSAGE( "modifying next element, which is moved back" );
   modify< Object >( 1/*id*/, id_idx3, [&]( Object& obj ){ obj.val = 2; } );
   modify< Object >( 1/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 2; } );
   it.get_modifier().add_modify( 1, 2 );
   /*
      id = 4; val = 0; val2 = 6; val3 = 10;
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 1; val = 2; val2 = 3; val3 = 21;
      id = 0; val = 5; val2 = 1; val3 = 0;   X
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
   */
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 4; val = 0; val2 = 6; val3 = 10;
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 1; val = 2; val2 = 3; val3 = 21;
      id = 0; val = 5; val2 = 1; val3 = 0;
      id = 2; val = 6; val2 = 4; val3 = 22;  X
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
   */
   BOOST_TEST_MESSAGE( "modifying next element, which is moved forward" );
   modify< Object >( 7/*id*/, id_idx2, [&]( Object& obj ){ obj.val = 20; } );
   modify< Object >( 7/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 20; } );
   it.get_modifier().add_modify( 1, 1 );
   /*
      id = 4; val = 0; val2 = 6; val3 = 10;
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 1; val = 2; val2 = 3; val3 = 21;
      id = 0; val = 5; val2 = 1; val3 = 0;
      id = 2; val = 6; val2 = 4; val3 = 22;  X
      id = 3; val = 9; val2 = 4; val3 = 3;
      id = 7; val = 20; val2 = 2; val3 = 13;
   */
   BOOST_REQUIRE( *it == *it_comparer );
   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );
}

template< bool Forward, typename Object, typename CMP, typename ID_Index, typename Index, typename Collection, typename Iterator >
void modification_test_3()
{
   Collection bmic1;
   Collection bmic2;
   Collection bmic3;

   Collection comparer;

   fill_for_modification_tests< Object >( bmic1, bmic2, bmic3, comparer );

   auto& id_idx1 = bmic1.template get< ID_Index >();
   auto& id_idx2 = bmic2.template get< ID_Index >();
   auto& id_idx3 = bmic3.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();
   const auto& idx3 = bmic3.template get< Index >();

   Iterator it( CMP(),
               std::make_tuple( &idx1, &id_idx1 ),
               std::make_tuple( &idx2, &id_idx2 ),
               std::make_tuple( &idx3, &id_idx3 )
            );

   Iterator it_end = Iterator::create_end( CMP(),
                                          std::make_tuple( &idx1, &id_idx1 ),
                                          std::make_tuple( &idx2, &id_idx2 ),
                                          std::make_tuple( &idx3, &id_idx3 )
            );

   auto& id_comparer_idx = comparer.template get< ID_Index >();
   auto& comparer_idx = comparer.template get< Index >();

   helper_iterator< Collection, Index, t_direction< Forward, Collection, Index > > hi;
   auto it_comparer = hi.get_begin( comparer_idx );

   /*
      id = 4; val = 0; val2 = 6; val3 = 10;  X
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 0; val = 5; val2 = 1; val3 = 0;
      id = 1; val = 5; val2 = 3; val3 = 21;
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
   */
   BOOST_TEST_MESSAGE( "modifying element on 'begin' position, which is moved forward" );
   modify< Object >( 4/*id*/, id_idx2, [&]( Object& obj ){ obj.val = 4; } );
   modify< Object >( 4/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 4; } );
   it.get_modifier().add_modify( 4, 1 );
   /*
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 4; val = 4; val2 = 6; val3 = 10;  X
      id = 0; val = 5; val2 = 1; val3 = 0;
      id = 1; val = 5; val2 = 3; val3 = 21;
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
   */
   BOOST_REQUIRE( *it == *it_comparer );

   BOOST_TEST_MESSAGE( "modifying next element, which is moved forward" );
   modify< Object >( 0/*id*/, id_idx1, [&]( Object& obj ){ obj.val = 10; } );
   modify< Object >( 0/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 10; } );
   it.get_modifier().add_modify( 0, 0 );
   /*
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 4; val = 4; val2 = 6; val3 = 10;  X
      id = 1; val = 5; val2 = 3; val3 = 21;
      id = 2; val = 6; val2 = 4; val3 = 22;
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
      id = 0; val = 10; val2 = 1; val3 = 0;
   */
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );
   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 5; val = 1; val2 = 6; val3 = 20;
      id = 4; val = 4; val2 = 6; val3 = 10;
      id = 1; val = 5; val2 = 3; val3 = 21;
      id = 2; val = 6; val2 = 4; val3 = 22;  X
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
      id = 0; val = 10; val2 = 1; val3 = 0;
   */
   BOOST_TEST_MESSAGE( "modifying previous element, which is moved forward" );
   modify< Object >( 5/*id*/, id_idx3, [&]( Object& obj ){ obj.val = 11; } );
   modify< Object >( 5/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 11; } );
   it.get_modifier().add_modify( 5, 2 );
   /*
      id = 4; val = 4; val2 = 6; val3 = 10;
      id = 1; val = 5; val2 = 3; val3 = 21;
      id = 2; val = 6; val2 = 4; val3 = 22;  X
      id = 7; val = 7; val2 = 2; val3 = 13;
      id = 3; val = 9; val2 = 4; val3 = 3;
      id = 0; val = 10; val2 = 1; val3 = 0;
      id = 5; val = 11; val2 = 6; val3 = 20;
   */
   while( it != it_end )
   {
      BOOST_REQUIRE( *it == *it_comparer );
      ++it;
      ++it_comparer;
   }
}

template< typename Object, typename Collection >
void fill_for_modification_tests( Collection& c1, Collection& c2, Collection& comparer )
{
   //bmic1
   auto c0_a = []( Object& obj ){ obj.id = 0; obj.val = 0; obj.val2 = 1; obj.val3 = 0; };
   auto c1_a = []( Object& obj ){ obj.id = 1; obj.val = 2; obj.val2 = 2; obj.val3 = 1; };
   auto c2_a = []( Object& obj ){ obj.id = 2; obj.val = 4; obj.val2 = 3; obj.val3 = 2; };
   auto c3_a = []( Object& obj ){ obj.id = 3; obj.val = 6; obj.val2 = 4; obj.val3 = 3; };
   auto c4_a = []( Object& obj ){ obj.id = 4; obj.val = 8; obj.val2 = 5; obj.val3 = 4; };
   auto c5_a = []( Object& obj ){ obj.id = 5; obj.val = 10; obj.val2 = 6; obj.val3 = 5; };
   c1.emplace( Object ( c0_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c1_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c2_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c3_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c4_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c5_a, std::allocator< Object >() ) );

   //bmic2
   auto c0_b = []( Object& obj ){ obj.id = 6; obj.val = 11; obj.val2 = 7; obj.val3 = 6; };
   auto c1_b = []( Object& obj ){ obj.id = 7; obj.val = 9; obj.val2 = 8; obj.val3 = 7; };
   auto c2_b = []( Object& obj ){ obj.id = 8; obj.val = 7; obj.val2 = 9; obj.val3 = 8; };
   auto c3_b = []( Object& obj ){ obj.id = 9; obj.val = 5; obj.val2 = 10; obj.val3 = 9; };
   auto c4_b = []( Object& obj ){ obj.id = 10; obj.val = 3; obj.val2 = 11; obj.val3 = 10; };
   auto c5_b = []( Object& obj ){ obj.id = 11; obj.val = 1; obj.val2 = 12; obj.val3 = 11; };
   c2.emplace( Object ( c0_b, std::allocator< Object >() ) );
   c2.emplace( Object ( c1_b, std::allocator< Object >() ) );
   c2.emplace( Object ( c2_b, std::allocator< Object >() ) );
   c2.emplace( Object ( c3_b, std::allocator< Object >() ) );
   c2.emplace( Object ( c4_b, std::allocator< Object >() ) );
   c2.emplace( Object ( c5_b, std::allocator< Object >() ) );

   //comparer
   auto c0_ = []( Object& obj ){ obj.id = 0; obj.val = 0; obj.val2 = 1; obj.val3 = 0; };
   auto c1_ = []( Object& obj ){ obj.id = 11; obj.val = 1; obj.val2 = 12; obj.val3 = 11; };
   auto c2_ = []( Object& obj ){ obj.id = 1; obj.val = 2; obj.val2 = 2; obj.val3 = 1; };
   auto c3_ = []( Object& obj ){ obj.id = 10; obj.val = 3; obj.val2 = 11; obj.val3 = 10; };
   auto c4_ = []( Object& obj ){ obj.id = 2; obj.val = 4; obj.val2 = 3; obj.val3 = 2; };
   auto c5_ = []( Object& obj ){ obj.id = 9; obj.val = 5; obj.val2 = 10; obj.val3 = 9; };
   auto c6_ = []( Object& obj ){ obj.id = 3; obj.val = 6; obj.val2 = 4; obj.val3 = 3; };
   auto c7_ = []( Object& obj ){ obj.id = 8; obj.val = 7; obj.val2 = 9; obj.val3 = 8; };
   auto c8_ = []( Object& obj ){ obj.id = 4; obj.val = 8; obj.val2 = 5; obj.val3 = 4; };
   auto c9_ = []( Object& obj ){ obj.id = 7; obj.val = 9; obj.val2 = 8; obj.val3 = 7; };
   auto c10_ = []( Object& obj ){ obj.id = 5; obj.val = 10; obj.val2 = 6; obj.val3 = 5; };
   auto c11_ = []( Object& obj ){ obj.id = 6; obj.val = 11; obj.val2 = 7; obj.val3 = 6; };

   comparer.emplace( Object ( c0_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c1_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c2_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c3_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c4_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c5_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c6_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c7_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c8_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c9_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c10_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c11_, std::allocator< Object >() ) );
}

template< typename Object, typename Collection >
void simple_fill_for_modification_tests( Collection& c1, Collection& comparer )
{
   //bmic1
   auto c0_a = []( Object& obj ){ obj.id = 0; obj.val = 5; obj.val2 = 1; obj.val3 = 0; };
   auto c1_a = []( Object& obj ){ obj.id = 1; obj.val = 8; obj.val2 = 4; obj.val3 = 2; };
   auto c2_a = []( Object& obj ){ obj.id = 2; obj.val = 9; obj.val2 = 4; obj.val3 = 3; };
   c1.emplace( Object ( c0_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c1_a, std::allocator< Object >() ) );
   c1.emplace( Object ( c2_a, std::allocator< Object >() ) );

   //comparer
   auto c0_ = []( Object& obj ){ obj.id = 0; obj.val = 5; obj.val2 = 1; obj.val3 = 0; };
   auto c1_ = []( Object& obj ){ obj.id = 1; obj.val = 8; obj.val2 = 4; obj.val3 = 2; };
   auto c2_ = []( Object& obj ){ obj.id = 2; obj.val = 9; obj.val2 = 4; obj.val3 = 3; };

   comparer.emplace( Object ( c0_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c1_, std::allocator< Object >() ) );
   comparer.emplace( Object ( c2_, std::allocator< Object >() ) );
}

template< bool Forward, typename Object, typename CMP, typename ID_Index, typename Index, typename Collection, typename Iterator >
void modification_test_forward()
{
   Collection bmic1;
   Collection bmic2;

   Collection comparer;

   simple_fill_for_modification_tests< Object >( bmic1, comparer );

   auto& id_idx1 = bmic1.template get< ID_Index >();
   auto& id_idx2 = bmic2.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();

   Iterator it( CMP(),
               std::make_tuple( &idx1, &id_idx1 ),
               std::make_tuple( &idx2, &id_idx2 )
            );

   auto& id_comparer_idx = comparer.template get< ID_Index >();
   auto& comparer_idx = comparer.template get< Index >();

   helper_iterator< Collection, Index, t_direction< Forward, Collection, Index > > hi;
   auto it_comparer = hi.get_begin( comparer_idx );

/*
   id = 0; val = 5; val2 = 1; val3 = 0;
   id = 1; val = 8; val2 = 4; val3 = 2;
   id = 2; val = 9; val2 = 4; val3 = 3;
*/
   BOOST_TEST_MESSAGE( "modifying next element, which is moved forward" );
   modify< Object >( 2/*id*/, id_idx1, [&]( Object& obj ){ obj.val = 4; } );
   modify< Object >( 2/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 4; } );
   it.get_modifier().add_modify( 0, 0 );

   BOOST_REQUIRE( *it == *it_comparer );
}

template< bool Forward, typename Object, typename CMP, typename ID_Index, typename Index, typename Collection, typename Iterator >
void modification_test_reverse()
{
   Collection bmic1;
   Collection bmic2;

   Collection comparer;

   simple_fill_for_modification_tests< Object >( bmic1, comparer );

   auto& id_idx1 = bmic1.template get< ID_Index >();
   auto& id_idx2 = bmic2.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();

   Iterator it( CMP(),
               std::make_tuple( &idx1, &id_idx1 ),
               std::make_tuple( &idx2, &id_idx2 )
            );

   auto& id_comparer_idx = comparer.template get< ID_Index >();
   auto& comparer_idx = comparer.template get< Index >();

   helper_iterator< Collection, Index, t_direction< Forward, Collection, Index > > hi;
   auto it_comparer = hi.get_begin( comparer_idx );

/*
   id = 0; val = 5; val2 = 1; val3 = 0;
   id = 1; val = 8; val2 = 4; val3 = 2;
   id = 2; val = 9; val2 = 4; val3 = 3;
*/
   BOOST_TEST_MESSAGE( "modifying next element, which is moved forward" );
   modify< Object >( 0/*id*/, id_idx1, [&]( Object& obj ){ obj.val = 13; } );
   modify< Object >( 0/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 13; } );
   it.get_modifier().add_modify( 0, 0 );

   BOOST_REQUIRE( *it == *it_comparer );
}

template< bool Forward, typename Object, typename CMP, typename ID_Index, typename Index, typename Collection, typename Iterator >
void modification_test_4()
{
   Collection bmic1;
   Collection bmic2;

   Collection comparer;

   fill_for_modification_tests< Object >( bmic1, bmic2, comparer );

   auto& id_idx1 = bmic1.template get< ID_Index >();
   auto& id_idx2 = bmic2.template get< ID_Index >();

   const auto& idx1 = bmic1.template get< Index >();
   const auto& idx2 = bmic2.template get< Index >();

   Iterator it( CMP(),
               std::make_tuple( &idx1, &id_idx1 ),
               std::make_tuple( &idx2, &id_idx2 )
            );

   Iterator it_end = Iterator::create_end( CMP(),
                                          std::make_tuple( &idx1, &id_idx1 ),
                                          std::make_tuple( &idx2, &id_idx2 )
            );

   auto& id_comparer_idx = comparer.template get< ID_Index >();
   auto& comparer_idx = comparer.template get< Index >();

   helper_iterator< Collection, Index, t_direction< Forward, Collection, Index > > hi;
   auto it_comparer = hi.get_begin( comparer_idx );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 0;  val = 0; val2 = 1;  val3 = 0;    (20)
      id = 11; val = 1; val2 = 12; val3 = 11;
      id = 1;  val = 2; val2 = 2;  val3 = 1;    (19)  X
      id = 10; val = 3; val2 = 11; val3 = 10;
      id = 2;  val = 4; val2 = 3;  val3 = 2;    (18)
      id = 9;  val = 5; val2 = 10; val3 = 9;
      id = 3;  val = 6; val2 = 4;  val3 = 3;    (17)
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 4;  val = 8; val2 = 5;  val3 = 4;    (16)
      id = 7;  val = 9; val2 = 8;  val3 = 7;
      id = 5;  val = 10; val2 = 6; val3 = 5;    (15)
      id = 6;  val = 11; val2 = 7; val3 = 6;
   */
   BOOST_TEST_MESSAGE( "modifying all even elements" );
   for( int32_t i = 0; i < 6; ++i )
   {
      modify< Object >( i/*id*/, id_idx1, [&]( Object& obj ){ obj.val = 20 - i; } );
      modify< Object >( i/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 20 - i; } );
      it.get_modifier().add_modify( i, 0 );
      BOOST_REQUIRE( *it == *it_comparer );
   }
   /*
      id = 11; val = 1; val2 = 12; val3 = 11;
      id = 10; val = 3; val2 = 11; val3 = 10;
      id = 9;  val = 5; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 7;  val = 9; val2 = 8;  val3 = 7;
      id = 6;  val = 11; val2 = 7; val3 = 6;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 4;  val = 16; val2 = 5;  val3 = 4;
      id = 3;  val = 17; val2 = 4;  val3 = 3;
      id = 2;  val = 18; val2 = 3;  val3 = 2;
      id = 1;  val = 19; val2 = 2;  val3 = 1;   X
      id = 0;  val = 20; val2 = 1;  val3 = 0;
   */
  while( it != it_end )
  {
      BOOST_REQUIRE( *it == *it_comparer );
      ++it;
      ++it_comparer;
  }

   --it;
   --it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );
   /*
      id = 11; val = 1; val2 = 12; val3 = 11;
      id = 10; val = 3; val2 = 11; val3 = 10;
      id = 9;  val = 5; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 7;  val = 9; val2 = 8;  val3 = 7;
      id = 6;  val = 11; val2 = 7; val3 = 6;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 4;  val = 16; val2 = 5;  val3 = 4;   (40)
      id = 3;  val = 17; val2 = 4;  val3 = 3;   (30)
      id = 2;  val = 18; val2 = 3;  val3 = 2;   (20)
      id = 1;  val = 19; val2 = 2;  val3 = 1;   (10)
      id = 0;  val = 20; val2 = 1;  val3 = 0;   X
   */
   BOOST_TEST_MESSAGE( "modifying 4 elements for id<1-4>" );
   for( int32_t i = 1; i <= 4; ++i )
   {
      modify< Object >( i/*id*/, id_idx1, [&]( Object& obj ){ obj.val = i * 10; } );
      modify< Object >( i/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = i * 10; } );
      it.get_modifier().add_modify( i, 0 );
      BOOST_REQUIRE( *it == *it_comparer );
   }
   /*
      id = 11; val = 1; val2 = 12; val3 = 11;
      id = 10; val = 3; val2 = 11; val3 = 10;
      id = 9;  val = 5; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 7;  val = 9; val2 = 8;  val3 = 7;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 6;  val = 11; val2 = 7; val3 = 6;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 0;  val = 20; val2 = 1;  val3 = 0;   X
      id = 2;  val = 20; val2 = 3;  val3 = 2;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
      id = 4;  val = 40; val2 = 5;  val3 = 4;
   */
  for( int32_t i = 0; i < 2; ++i )
  {
      BOOST_REQUIRE( *it == *it_comparer );
      --it;
      --it_comparer;
  }

   /*
      id = 11; val = 1; val2 = 12; val3 = 11;
      id = 10; val = 3; val2 = 11; val3 = 10;
      id = 9;  val = 5; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 7;  val = 9; val2 = 8;  val3 = 7;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 6;  val = 11; val2 = 7; val3 = 6;    (2,0)X
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
      id = 4;  val = 40; val2 = 5;  val3 = 4;
   */
   BOOST_TEST_MESSAGE( "modifying 1 element for id<6>" );
   modify< Object >( 6/*id*/, id_idx2, [&]( Object& obj ){ obj.val = 2; obj.val2 = 0; } );
   modify< Object >( 6/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 2; obj.val2 = 0; } );
   it.get_modifier().add_modify( 6, 1 );
   BOOST_REQUIRE( *it == *it_comparer );
   /*
      id = 11; val = 1; val2 = 12; val3 = 11;
      id = 6;  val = 2; val2 = 0; val3 = 6;     X
      id = 10; val = 3; val2 = 11; val3 = 10;
      id = 9;  val = 5; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 7;  val = 9; val2 = 8;  val3 = 7;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
      id = 4;  val = 40; val2 = 5;  val3 = 4;
   */

   --it;
   --it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 11; val = 1; val2 = 12; val3 = 11;   (21)  X
      id = 6;  val = 2; val2 = 0; val3 = 6;     (17)
      id = 10; val = 3; val2 = 11; val3 = 10;   (13)
      id = 9;  val = 5; val2 = 10; val3 = 9;    (10)
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 7;  val = 9; val2 = 8;  val3 = 7;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
      id = 4;  val = 40; val2 = 5;  val3 = 4;
   */
   BOOST_TEST_MESSAGE( "modifying 4 elements for id<11,6,10,9>" );
   int id_set[]={ 11, 6, 10, 9 };
   for( int32_t i = 0; i < 4; ++i )
   {
      modify< Object >( id_set[i]/*id*/, id_idx2, [&]( Object& obj ){ obj.val = obj.val + 20 - 5*i; } );
      modify< Object >( id_set[i]/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = obj.val + 20 - 5*i; } );
      it.get_modifier().add_modify( id_set[i], 1 );
      BOOST_REQUIRE( *it == *it_comparer );
   }
   /*
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 7;  val = 9; val2 = 8;  val3 = 7;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 9;  val = 10; val2 = 10; val3 = 9;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;
      id = 11; val = 21; val2 = 12; val3 = 11;     X
      id = 3;  val = 30; val2 = 4;  val3 = 3;
      id = 4;  val = 40; val2 = 5;  val3 = 4;
   */

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( *it == *it_comparer );

   ++it;
   ++it_comparer;
   BOOST_REQUIRE( it == it_end );

   it--;
   it_comparer--;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 7;  val = 9; val2 = 8;  val3 = 7;       (0)
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 9;  val = 10; val2 = 10; val3 = 9;      (0)
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;
      id = 11; val = 21; val2 = 12; val3 = 11;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
      id = 4;  val = 40; val2 = 5;  val3 = 4;      (0)X
   */
   BOOST_TEST_MESSAGE( "modifying 3 elements for id<4,7,9>" );
   int id_set2[]={ 4, 7, 9 };
   for( int32_t i = 0; i < 3; ++i )
   {
      modify< Object >( id_set2[i]/*id*/, i==0?id_idx1:id_idx2, [&]( Object& obj ){ obj.val = 0; } );
      modify< Object >( id_set2[i]/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 0; } );
      it.get_modifier().add_modify( id_set2[i], i==0?0:1 );
      BOOST_REQUIRE( *it == *it_comparer );
   }
   /*
      id = 4;  val = 0; val2 = 5;  val3 = 4;       X
      id = 7;  val = 0; val2 = 8;  val3 = 7;
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;
      id = 11; val = 21; val2 = 12; val3 = 11;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
   */
   while( it != it_end )
   {
      BOOST_REQUIRE( *it == *it_comparer );
      ++it;
      ++it_comparer;
   }
   BOOST_REQUIRE( it == it_end );

   it--;
   it_comparer--;
   BOOST_REQUIRE( *it == *it_comparer );

   it--;
   it_comparer--;
   BOOST_REQUIRE( *it == *it_comparer );

   it--;
   it_comparer--;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 4;  val = 0; val2 = 5;  val3 = 4;
      id = 7;  val = 0; val2 = 8;  val3 = 7;
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;         X
      id = 11; val = 21; val2 = 12; val3 = 11;  (19)
      id = 3;  val = 30; val2 = 4;  val3 = 3;
   */
   BOOST_TEST_MESSAGE( "modifying 1 elements for id<11> and change direction from '--' to '++'" );
   modify< Object >( 11/*id*/, id_idx2, [&]( Object& obj ){ obj.val = 19; } );
   modify< Object >( 11/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 19; } );
   it.get_modifier().add_modify( 11, 1 );
   BOOST_REQUIRE( *it == *it_comparer );
   /*
      id = 4;  val = 0; val2 = 5;  val3 = 4;
      id = 7;  val = 0; val2 = 8;  val3 = 7;
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 11; val = 19; val2 = 12; val3 = 11;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;   X
      id = 3;  val = 30; val2 = 4;  val3 = 3;
   */

   it++;
   it_comparer++;
   BOOST_REQUIRE( *it == *it_comparer );

   /*
      id = 4;  val = 0; val2 = 5;  val3 = 4;
      id = 7;  val = 0; val2 = 8;  val3 = 7;
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 11; val = 19; val2 = 12; val3 = 11;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 2;  val = 20; val2 = 3;  val3 = 2;   (5)
      id = 3;  val = 30; val2 = 4;  val3 = 3;         X
   */

   BOOST_TEST_MESSAGE( "modifying 1 elements for id<2> and change direction from '++' to '--'" );
   modify< Object >( 2/*id*/, id_idx1, [&]( Object& obj ){ obj.val = 5; } );
   modify< Object >( 2/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 5; } );
   it.get_modifier().add_modify( 2, 0 );
   BOOST_REQUIRE( *it == *it_comparer );
   /*
      id = 4;  val = 0; val2 = 5;  val3 = 4;
      id = 7;  val = 0; val2 = 8;  val3 = 7;
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 2;  val = 5; val2 = 3;  val3 = 2;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 11; val = 19; val2 = 12; val3 = 11;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
   */

   for( int i = 0; i < 10; i++ )
   {
      --it;
      it_comparer--;
      BOOST_REQUIRE( *it == *it_comparer );
   }

   /*
      id = 4;  val = 0; val2 = 5;  val3 = 4;    (4)
      id = 7;  val = 0; val2 = 8;  val3 = 7;          X
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 2;  val = 5; val2 = 3;  val3 = 2;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 11; val = 19; val2 = 12; val3 = 11;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
   */
   BOOST_TEST_MESSAGE( "modifying 1 elements for id<4> and change direction from '--' to '++'" );
   modify< Object >( 4/*id*/, id_idx1, [&]( Object& obj ){ obj.val = 4; } );
   modify< Object >( 4/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 4; } );
   it.get_modifier().add_modify( 4, 0 );
   BOOST_REQUIRE( *it == *it_comparer );
   /*
      id = 7;  val = 0; val2 = 8;  val3 = 7;          X
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 4;  val = 4; val2 = 5;  val3 = 4;
      id = 2;  val = 5; val2 = 3;  val3 = 2;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 1;  val = 10; val2 = 2;  val3 = 1;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 11; val = 19; val2 = 12; val3 = 11;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
   */
   for( int i = 0; i < 5; i++ )
   {
      it++;
      ++it_comparer;
      BOOST_REQUIRE( *it == *it_comparer );
   }

   /*
      id = 7;  val = 0; val2 = 8;  val3 = 7;
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 4;  val = 4; val2 = 5;  val3 = 4;
      id = 2;  val = 5; val2 = 3;  val3 = 2;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 1;  val = 10; val2 = 2;  val3 = 1;      (0)    X
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 11; val = 19; val2 = 12; val3 = 11;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
   */
   BOOST_TEST_MESSAGE( "modifying 1 elements for id<1>" );
   modify< Object >( 1/*id*/, id_idx1, [&]( Object& obj ){ obj.val = 0; } );
   modify< Object >( 1/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 0; } );
   it.get_modifier().add_modify( 1, 0 );
   BOOST_REQUIRE( *it == *it_comparer );
   /*
      id = 1;  val = 0; val2 = 2;  val3 = 1;       X
      id = 7;  val = 0; val2 = 8;  val3 = 7;
      id = 9;  val = 0; val2 = 10; val3 = 9;
      id = 4;  val = 4; val2 = 5;  val3 = 4;
      id = 2;  val = 5; val2 = 3;  val3 = 2;
      id = 8;  val = 7; val2 = 9;  val3 = 8;
      id = 10; val = 13; val2 = 11; val3 = 10;
      id = 5;  val = 15; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 11; val = 19; val2 = 12; val3 = 11;
      id = 0;  val = 20; val2 = 1;  val3 = 0;
      id = 3;  val = 30; val2 = 4;  val3 = 3;
   */

   while( it != it_end )
   {
      BOOST_REQUIRE( *it == *it_comparer );
      it++;
      ++it_comparer;
   }
   BOOST_REQUIRE( it == it_end );

   /*
      id = 1;  val = 0; val2 = 2;  val3 = 1;    (40)
      id = 7;  val = 0; val2 = 8;  val3 = 7;    (39)
      id = 9;  val = 0; val2 = 10; val3 = 9;    (38)
      id = 4;  val = 4; val2 = 5;  val3 = 4;    (37)
      id = 2;  val = 5; val2 = 3;  val3 = 2;    (36)
      id = 8;  val = 7; val2 = 9;  val3 = 8;    (35)
      id = 10; val = 13; val2 = 11; val3 = 10;  (34)
      id = 5;  val = 15; val2 = 6; val3 = 5;    (33)
      id = 6;  val = 17; val2 = 0; val3 = 6;    (32)
      id = 11; val = 19; val2 = 12; val3 = 11;  (31)
      id = 0;  val = 20; val2 = 1;  val3 = 0;   (30)
      id = 3;  val = 30; val2 = 4;  val3 = 3;   (29)
                                                   X
   */
   BOOST_TEST_MESSAGE( "modifying all elements" );
   int id_set3[]={ 1, 7, 9, 4, 2, 8, 10, 5, 6, 11, 0, 3 };
   for( int32_t i = 0; i < 12; ++i )
   {
      modify< Object >( id_set3[i]/*id*/, ( id_set3[i] < 6 )?id_idx1:id_idx2, [&]( Object& obj ){ obj.val = 40 - i; } );
      modify< Object >( id_set3[i]/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 40 - i; } );
      it.get_modifier().add_modify( id_set3[i], ( id_set3[i] < 6 )?0:1 );
   }
   /*
      id = 3;  val = 29; val2 = 4;  val3 = 3;
      id = 0;  val = 30; val2 = 1;  val3 = 0;
      id = 11; val = 31; val2 = 12; val3 = 11;
      id = 6;  val = 32; val2 = 0; val3 = 6;
      id = 5;  val = 33; val2 = 6; val3 = 5;
      id = 10; val = 34; val2 = 11; val3 = 10;
      id = 8;  val = 35; val2 = 9;  val3 = 8;
      id = 2;  val = 36; val2 = 3;  val3 = 2;
      id = 4;  val = 37; val2 = 5;  val3 = 4;
      id = 9;  val = 38; val2 = 10; val3 = 9;
      id = 7;  val = 39; val2 = 8;  val3 = 7;
      id = 1;  val = 40; val2 = 2;  val3 = 1;
                                                   X
   */

   for( int i = 0; i < 12; i++ )
   {
      --it;
      --it_comparer;
      BOOST_REQUIRE( *it == *it_comparer );
   }

   /*
      id = 3;  val = 29; val2 = 4;  val3 = 3;      (20)X
      id = 0;  val = 30; val2 = 1;  val3 = 0;      (19)
      id = 11; val = 31; val2 = 12; val3 = 11;     (18)
      id = 6;  val = 32; val2 = 0; val3 = 6;       (17)
      id = 5;  val = 33; val2 = 6; val3 = 5;       (16)
      id = 10; val = 34; val2 = 11; val3 = 10;     (15)
      id = 8;  val = 35; val2 = 9;  val3 = 8;      (14)
      id = 2;  val = 36; val2 = 3;  val3 = 2;      (13)
      id = 4;  val = 37; val2 = 5;  val3 = 4;      (12)
      id = 9;  val = 38; val2 = 10; val3 = 9;      (11)
      id = 7;  val = 39; val2 = 8;  val3 = 7;      (10)
      id = 1;  val = 40; val2 = 2;  val3 = 1;      (09)
   */
   BOOST_TEST_MESSAGE( "modifying all elements" );
   int id_set4[]={ 3, 0, 11, 6, 5, 10, 8, 2, 4, 9, 7, 1 };
   for( int32_t i = 0; i < 12; ++i )
   {
      modify< Object >( id_set4[i]/*id*/, ( id_set4[i] < 6 )?id_idx1:id_idx2, [&]( Object& obj ){ obj.val = 20 - i; } );
      modify< Object >( id_set4[i]/*id*/, id_comparer_idx, [&]( Object& obj ){ obj.val = 20 - i; } );
      it.get_modifier().add_modify( id_set4[i], ( id_set4[i] < 6 )?0:1 );
   }
   /*
      id = 1;  val = 09; val2 = 2;  val3 = 1;
      id = 7;  val = 10; val2 = 8;  val3 = 7;
      id = 9;  val = 11; val2 = 10; val3 = 9;
      id = 4;  val = 12; val2 = 5;  val3 = 4;
      id = 2;  val = 13; val2 = 3;  val3 = 2;
      id = 8;  val = 14; val2 = 9;  val3 = 8;
      id = 10; val = 15; val2 = 11; val3 = 10;
      id = 5;  val = 16; val2 = 6; val3 = 5;
      id = 6;  val = 17; val2 = 0; val3 = 6;
      id = 11; val = 18; val2 = 12; val3 = 11;
      id = 0;  val = 19; val2 = 1;  val3 = 0;
      id = 3;  val = 20; val2 = 4;  val3 = 3;         X
   */
   while( it != it_end )
   {
      BOOST_REQUIRE( *it == *it_comparer );
      it++;
      ++it_comparer;
   }
   BOOST_REQUIRE( it == it_end );
}

BOOST_AUTO_TEST_SUITE(concatenation_enumeration_tests)

BOOST_AUTO_TEST_CASE(modification_tests_others)
{
   using Object = ce_tests::test_object;
   using CMP = ce_tests::cmp2;
   using ID_Index = ce_tests::OrderedIndex;
   using Index = ce_tests::CompositeOrderedIndexA;
   using Collection = ce_tests::test_object_index;

   using Iterator = ce::concatenation_iterator_ex< Object, CMP >;
   using ReverseIterator = ce::concatenation_reverse_iterator_ex< Object, CMP >;

   modification_test_forward< true, Object, CMP, ID_Index, Index, Collection, Iterator >();
   modification_test_reverse< false, Object, CMP, ID_Index, Index, Collection, ReverseIterator >();
}

BOOST_AUTO_TEST_CASE(modification_tests_forward)
{
   using Object = ce_tests::test_object;
   using CMP = ce_tests::cmp2;
   using ID_Index = ce_tests::OrderedIndex;
   using Index = ce_tests::CompositeOrderedIndexA;
   using Collection = ce_tests::test_object_index;

   using Iterator = ce::concatenation_iterator_ex< Object, CMP >;

   modification_test_1< true, Object, CMP, ID_Index, Index, Collection, Iterator >();
   modification_test_2< true, Object, CMP, ID_Index, Index, Collection, Iterator >();
   modification_test_3< true, Object, CMP, ID_Index, Index, Collection, Iterator >();
   modification_test_4< true, Object, CMP, ID_Index, Index, Collection, Iterator >();
}

BOOST_AUTO_TEST_CASE(modification_tests_reverse)
{
   using Object = ce_tests::test_object;
   using CMP = ce_tests::cmp2;
   using ID_Index = ce_tests::OrderedIndex;
   using Index = ce_tests::CompositeOrderedIndexA;
   using Collection = ce_tests::test_object_index;

   using ReverseIterator = ce::concatenation_reverse_iterator_ex< Object, CMP >;

   modification_test_1< false, Object, CMP, ID_Index, Index, Collection, ReverseIterator >();
   modification_test_2< false, Object, CMP, ID_Index, Index, Collection, ReverseIterator >();
   //modification_test_3< false, Object, CMP, ID_Index, Index, Collection, ReverseIterator >();
   //modification_test_4< false, Object, CMP, ID_Index, Index, Collection, ReverseIterator >();
}

BOOST_AUTO_TEST_CASE(different_tests)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using oidx = ce_tests::OrderedIndex;

   {
      using cmp1 = ce_tests::cmp1;
      using iterator = ce::concatenation_iterator< obj, cmp1 >;
      using reverse_iterator = ce::concatenation_reverse_iterator< obj, cmp1 >;

      auto f1 = []( bmic& collection ){ ce_tests::fill6< obj >( collection ); };

      different_test< iterator, reverse_iterator, bmic, obj, oidx, cmp1 >( f1 );
   }

   {
      using cmp2 = ce_tests::cmp2;
      using oidx_a = ce_tests::CompositeOrderedIndexA;
      using iterator = ce::concatenation_iterator_ex< obj, cmp2 >;
      using reverse_iterator = ce::concatenation_reverse_iterator_ex< obj, cmp2 >;

      auto f1 = []( bmic& collection ){ ce_tests::fill9< obj >( collection ); };
      auto f2 = []( bmic& collection ){ ce_tests::fill9a< obj >( collection ); };
      auto f3 = []( bmic& collection ){ ce_tests::fill9b< obj >( collection ); };
      auto s = []( bmic& collection ){ ce_tests::sort9< obj >( collection ); };

      different_test_sub_index< iterator, reverse_iterator, bmic, obj, oidx, oidx_a, cmp2 >( f1, f2, f3, s );
   }
}

BOOST_AUTO_TEST_CASE(inc_dec_basic_reverse_tests)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp1 = ce_tests::cmp1;
   using oidx = ce_tests::OrderedIndex;

   auto f1 = []( bmic& collection ){ ce_tests::fill6< obj >( collection ); };
   using reverse_iterator = ce::concatenation_reverse_iterator< obj, cmp1 >;

   inc_dec_basic_reverse_test< reverse_iterator, bmic, obj, oidx, cmp1 >( f1 );
}

BOOST_AUTO_TEST_CASE(misc_operations_sub_index_tests)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp2 = ce_tests::cmp2;
   using oidx = ce_tests::OrderedIndex;
   using oidx_a = ce_tests::CompositeOrderedIndexA;

   auto f1 = []( bmic& collection ){ ce_tests::fill9< obj >( collection ); };
   auto f2 = []( bmic& collection ){ ce_tests::fill9a< obj >( collection ); };
   auto f3 = []( bmic& collection ){ ce_tests::fill9b< obj >( collection ); };
   auto s = []( bmic& collection ){ ce_tests::sort9< obj >( collection ); };

   misc_operations_sub_index_test< ce::concatenation_iterator_ex< obj, cmp2 >, bmic, obj, oidx, oidx_a, cmp2 >( f1, f2, f3, s );
}

BOOST_AUTO_TEST_CASE(comparision_assignment_sub_index_tests)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp2 = ce_tests::cmp2;
   using oidx = ce_tests::OrderedIndex;
   using oidx_a = ce_tests::CompositeOrderedIndexA;

   auto f1 = []( bmic& collection ){ ce_tests::fill9< obj >( collection ); };
   auto f2 = []( bmic& collection ){ ce_tests::fill9a< obj >( collection ); };
   auto f3 = []( bmic& collection ){ ce_tests::fill9b< obj >( collection ); };
   auto s = []( bmic& collection ){ ce_tests::sort9< obj >( collection ); };

   comparision_assignment_sub_index_test< ce::concatenation_iterator_ex< obj, cmp2 >, bmic, obj, oidx, oidx_a, cmp2 >( f1, f2, f3, s );
}

BOOST_AUTO_TEST_CASE(comparision_assignment_tests)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp1 = ce_tests::cmp1;
   using oidx = ce_tests::OrderedIndex;

   auto f1 = []( bmic& collection ){ ce_tests::fill7< obj >( collection ); };
   auto f2 = []( bmic& collection ){ ce_tests::fill7a< obj >( collection ); };
   auto f3 = []( bmic& collection ){ ce_tests::fill7b< obj >( collection ); };
   auto s = []( bmic& collection ){ ce_tests::sort7< obj >( collection ); };

   comparision_assignment_test< ce::concatenation_iterator< obj, cmp1 >, bmic, obj, oidx, cmp1 >( f1, f2, f3, s );
}

BOOST_AUTO_TEST_CASE(inc_dec_basic_tests3)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp2 = ce_tests::cmp2;
   using oidx = ce_tests::OrderedIndex;
   using oidx_a = ce_tests::CompositeOrderedIndexA;

   auto f1 = []( bmic& collection ){ ce_tests::fill9< obj >( collection ); };
   auto f2 = []( bmic& collection ){ ce_tests::fill9a< obj >( collection ); };
   auto f3 = []( bmic& collection ){ ce_tests::fill9b< obj >( collection ); };
   auto s = []( bmic& collection ){ ce_tests::sort9< obj >( collection ); };

   using iterator = ce::concatenation_iterator_ex< obj, cmp2 >;
   using reverse_iterator = ce::concatenation_reverse_iterator_ex< obj, cmp2 >;

   inc_dec_basic_3_sources_sub_index_test< iterator, reverse_iterator, bmic, obj, oidx, oidx_a, cmp2 >( f1, f2, f3, s );
}

BOOST_AUTO_TEST_CASE(inc_dec_basic_tests2)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp1 = ce_tests::cmp1;
   using oidx = ce_tests::OrderedIndex;

   auto f1 = []( bmic& collection ){ ce_tests::fill8< obj >( collection ); };
   auto f2 = []( bmic& collection ){ ce_tests::fill8a< obj >( collection ); };
   auto f3 = []( bmic& collection ){ ce_tests::fill8b< obj >( collection ); };
   auto f4 = []( bmic& collection ){ ce_tests::fill8c< obj >( collection ); };
   auto f5 = []( bmic& collection ){ ce_tests::fill8d< obj >( collection ); };
   auto f6 = []( bmic& collection ){ ce_tests::fill8e< obj >( collection ); };
   auto s = []( bmic& collection ){ ce_tests::sort8< obj >( collection ); };

   inc_dec_basic_6_sources_test< bmic, obj, oidx, cmp1 >( f1, f2, f3, f4, f5, f6, s );
}

BOOST_AUTO_TEST_CASE(inc_dec_basic_tests1)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp1 = ce_tests::cmp1;
   using oidx = ce_tests::OrderedIndex;

   auto f1 = []( bmic& collection ){ ce_tests::fill7< obj >( collection ); };
   auto f2 = []( bmic& collection ){ ce_tests::fill7a< obj >( collection ); };
   auto f3 = []( bmic& collection ){ ce_tests::fill7b< obj >( collection ); };
   auto s = []( bmic& collection ){ ce_tests::sort7< obj >( collection ); };

   inc_dec_basic_3_sources_test< bmic, obj, oidx, cmp1 >( f1, f2, f3, s );
}

BOOST_AUTO_TEST_CASE(inc_dec_basic_tests)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp1 = ce_tests::cmp1;
   using oidx = ce_tests::OrderedIndex;

   auto f1 = []( bmic& collection ){ ce_tests::fill6< obj >( collection ); };
   using iterator = ce::concatenation_iterator< obj, cmp1 >;
   using reverse_iterator = ce::concatenation_reverse_iterator< obj, cmp1 >;

   inc_dec_basic_1_source_test< iterator, reverse_iterator, bmic, obj, oidx, cmp1 >( f1 );
}

BOOST_AUTO_TEST_CASE(basic_tests)
{
   using obj = ce_tests::test_object;
   using bmic = ce_tests::test_object_index;
   using cmp1 = ce_tests::cmp1;
   using cmp2 = ce_tests::cmp2;
   using cmp3 = ce_tests::cmp3;
   using oidx = ce_tests::OrderedIndex;
   using c_oidx_a = ce_tests::CompositeOrderedIndexA;
   using c_oidx_b = ce_tests::CompositeOrderedIndexB;

   std::vector< obj > sorted;

   basic_test< std::set< obj, cmp1 >, obj >();

   ce_tests::sort1< obj >( sorted );
   test_different_sources< bmic, std::set< obj, cmp1 >, oidx, obj, cmp1 >( sorted );

   ce_tests::sort2< obj >( sorted );
   auto c1 = []( bmic& collection ){ ce_tests::fill2< obj >( collection ); };
   auto c2 = []( bmic& collection ){ ce_tests::fill2b< obj >( collection ); };
   test_with_sub_index<
                  decltype( c1 ),
                  decltype( c2 ),
                  bmic,
                  oidx,
                  c_oidx_a,
                  obj,
                  cmp2
                  >( c1, c2, sorted );

   ce_tests::sort4< obj >( sorted );
   test_with_sub_index_3_sources<
                  bmic,
                  std::set< obj, cmp2 >,
                  oidx,
                  c_oidx_a,
                  obj,
                  cmp2
                  >( sorted );

   ce_tests::sort4a< obj >( sorted );
   test_with_sub_index_3_sources<
                  bmic,
                  std::set< obj, cmp3 >,
                  oidx,
                  c_oidx_b,
                  obj,
                  cmp3
                  >( sorted );

   ce_tests::sort5< obj >( sorted );
   auto c1a = []( bmic& collection ){ ce_tests::fill5< obj >( collection ); };
   auto c2a = []( bmic& collection ){ ce_tests::fill5b< obj >( collection ); };
   test_with_sub_index<
                  decltype( c1a ),
                  decltype( c2a ),
                  bmic,
                  oidx,
                  c_oidx_a,
                  obj,
                  cmp2
                  >( c1a, c2a, sorted );
}

BOOST_AUTO_TEST_CASE(advanced_tests)
{
   using obj2 = ce_tests::test_object2;
   using bmic2 = ce_tests::test_object_index2;
   using oidx2 = ce_tests::OrderedIndex2;
   using c_oidx2 = ce_tests::CompositeOrderedIndex2;
   using cmp4 = ce_tests::cmp4;

   test_with_sub_index_2_sources_many_objects_without_id_repeat
   <
      bmic2,
      oidx2,
      c_oidx2,
      obj2,
      cmp4
   >();

   test_with_sub_index_2_sources_many_objects_with_id_repeat
   <
      bmic2,
      oidx2,
      c_oidx2,
      obj2,
      cmp4
   >();

   test_with_sub_index_3_sources_many_objects_with_id_repeat
   <
      bmic2,
      oidx2,
      c_oidx2,
      obj2,
      cmp4
   >();
}

BOOST_AUTO_TEST_CASE(benchmark_tests)
{
   using obj2 = ce_tests::test_object2;
   using bmic2 = ce_tests::test_object_index2;
   using oidx2 = ce_tests::OrderedIndex2;
   using c_oidx2 = ce_tests::CompositeOrderedIndex2;
   using cmp4 = ce_tests::cmp4;

   benchmark_test_2_sources
   <
      false/*Is_Another_Source*/,
      bmic2,
      oidx2,
      c_oidx2,
      obj2,
      cmp4
   >();

   benchmark_test_3_sources
   <
      false/*Is_Another_Source*/,
      bmic2,
      oidx2,
      c_oidx2,
      obj2,
      cmp4
   >();

   benchmark_test_2_sources
   <
      true/*Is_Another_Source*/,
      bmic2,
      oidx2,
      c_oidx2,
      obj2,
      cmp4
   >();

   benchmark_test_3_sources
   <
      true/*Is_Another_Source*/,
      bmic2,
      oidx2,
      c_oidx2,
      obj2,
      cmp4
   >();
/*
Checked on hardware:
	16 GB RAM
	Intel® Core™ i5-7400 CPU @ 3.00GHz × 4
	OS-type 64 bit

************************************************************************************************************************
'benchmark_test_2_sources - Is_Another_Source( false )'
`2 levels( persistent_state( 1mln objects ) + volatile state( 100 000 objects ) -> keys searching in collection with length: 100 000 objects`
iterator: bmic iterator: 33 ms
iterator: concatenation iterator: 143 ms
ratio: 4.33333
reverse_iterator: bmic iterator: 42 ms
reverse_iterator: concatenation iterator: 126 ms
ratio: 3

'benchmark_test_2_sources - Is_Another_Source( true )'
2 levels( persistent_state( 1mln objects ) + volatile state( 100 000 objects ) -> keys searching in collection with length: 0 objects
iterator: bmic iterator: 39 ms
iterator: concatenation iterator: 77 ms
ratio: 1.97436
reverse_iterator: bmic iterator: 44 ms
reverse_iterator: concatenation iterator: 62 ms
ratio: 1.40909

************************************************************************************************************************

'benchmark_test_3_sources - Is_Another_Source( false )'
`3 levels( persistent_state( 1mln objects ) + volatile state( 100 000 objects ) + volatile state( 100 000 objects ) -> keys searching in 2 collections - every has 100 000 objects`
iterator: bmic iterator: 48 ms
iterator: concatenation iterator: 354 ms
ratio: 7.375
reverse_iterator: bmic iterator: 44 ms
reverse_iterator: concatenation iterator: 260 ms
ratio: 5.90909

'benchmark_test_3_sources - Is_Another_Source( true )'
`3 levels( persistent_state( 1mln objects ) + volatile state( 100 000 objects ) + volatile state( 100 000 objects ) -> keys searching in 2 collections - every has 0 objects`
iterator: bmic iterator: 46 ms
iterator: concatenation iterator: 246 ms
ratio: 5.34783
reverse_iterator: bmic iterator: 45 ms
reverse_iterator: concatenation iterator: 153 ms
ratio: 3.4
*/
}

BOOST_AUTO_TEST_SUITE_END()
