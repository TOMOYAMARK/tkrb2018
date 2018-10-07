// Generated by gencpp from file tr_main/AddTwoInts.msg
// DO NOT EDIT!


#ifndef TR_MAIN_MESSAGE_ADDTWOINTS_H
#define TR_MAIN_MESSAGE_ADDTWOINTS_H

#include <ros/service_traits.h>


#include <tr_main/AddTwoIntsRequest.h>
#include <tr_main/AddTwoIntsResponse.h>


namespace tr_main
{

struct AddTwoInts
{

typedef AddTwoIntsRequest Request;
typedef AddTwoIntsResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;

}; // struct AddTwoInts
} // namespace tr_main


namespace ros
{
namespace service_traits
{


template<>
struct MD5Sum< ::tr_main::AddTwoInts > {
  static const char* value()
  {
    return "6a2e34150c00229791cc89ff309fff21";
  }

  static const char* value(const ::tr_main::AddTwoInts&) { return value(); }
};

template<>
struct DataType< ::tr_main::AddTwoInts > {
  static const char* value()
  {
    return "tr_main/AddTwoInts";
  }

  static const char* value(const ::tr_main::AddTwoInts&) { return value(); }
};


// service_traits::MD5Sum< ::tr_main::AddTwoIntsRequest> should match 
// service_traits::MD5Sum< ::tr_main::AddTwoInts > 
template<>
struct MD5Sum< ::tr_main::AddTwoIntsRequest>
{
  static const char* value()
  {
    return MD5Sum< ::tr_main::AddTwoInts >::value();
  }
  static const char* value(const ::tr_main::AddTwoIntsRequest&)
  {
    return value();
  }
};

// service_traits::DataType< ::tr_main::AddTwoIntsRequest> should match 
// service_traits::DataType< ::tr_main::AddTwoInts > 
template<>
struct DataType< ::tr_main::AddTwoIntsRequest>
{
  static const char* value()
  {
    return DataType< ::tr_main::AddTwoInts >::value();
  }
  static const char* value(const ::tr_main::AddTwoIntsRequest&)
  {
    return value();
  }
};

// service_traits::MD5Sum< ::tr_main::AddTwoIntsResponse> should match 
// service_traits::MD5Sum< ::tr_main::AddTwoInts > 
template<>
struct MD5Sum< ::tr_main::AddTwoIntsResponse>
{
  static const char* value()
  {
    return MD5Sum< ::tr_main::AddTwoInts >::value();
  }
  static const char* value(const ::tr_main::AddTwoIntsResponse&)
  {
    return value();
  }
};

// service_traits::DataType< ::tr_main::AddTwoIntsResponse> should match 
// service_traits::DataType< ::tr_main::AddTwoInts > 
template<>
struct DataType< ::tr_main::AddTwoIntsResponse>
{
  static const char* value()
  {
    return DataType< ::tr_main::AddTwoInts >::value();
  }
  static const char* value(const ::tr_main::AddTwoIntsResponse&)
  {
    return value();
  }
};

} // namespace service_traits
} // namespace ros

#endif // TR_MAIN_MESSAGE_ADDTWOINTS_H
