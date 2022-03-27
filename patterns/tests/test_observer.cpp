#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "observer/observer.h"

#include <memory>

using namespace dp;

struct Newspaper {
  std::string news{};
};

void freeNewspapaper(const Newspaper &) {}

using Observer = BasicObserver<Newspaper>;

struct Reader : public Observer::Subscriber {
  explicit Reader(const Observer::SubjectPtr &a_subject)
    : Observer::Subscriber{ a_subject }
  {
  }
  void readTime(const Newspaper &a_newsPaper)
  {
    ++timeRead;
    ++messageReceived;
  }

  void readWeather(const Newspaper &a_newsPaper)
  {
    ++weatherRead;
    ++messageReceived;
  }

  int messageReceived{};
  int timeRead{};
  int weatherRead{};
  bool freeFunctionCalled{};
};

TEST_CASE("Simple observer", "[Observer]")
{
  GIVEN("Publisher and reader for newspaper")
  {
    Observer::SubjectPtr publisher = std::make_shared<Observer::Subject>();
    Reader reader{ publisher };

    WHEN("Reader is not subscribed and publisher publish something")
    {
      publisher->notify("TIME", Newspaper{ "Breaking news" });
      THEN("Reader won't receive any message") { REQUIRE(reader.messageReceived == 0); }
    }

    WHEN("Reader subscribe on 2 topics")
    {
      reader.subscribe("TIME", &Reader::readTime);
      reader.subscribe("WEATHER", &Reader::readWeather);
      AND_WHEN("Publisher publish on topic that was subscribed")
      {
        publisher->notify("TIME", Newspaper{ "Breaking news!" });
        THEN("Reader receives a message")
        {
          REQUIRE(reader.messageReceived == 1);
          REQUIRE(reader.timeRead == 1);
          REQUIRE(reader.weatherRead == 0);
        }
      }
      AND_WHEN("Publisher publish on topic that was not subscribed")
      {
        publisher->notify("SPORT", Newspaper{ "New record..." });
        THEN("Reader does not receive a message")
        {
          REQUIRE(reader.messageReceived == 0);
          REQUIRE(reader.timeRead == 0);
          REQUIRE(reader.weatherRead == 0);
        }
      }
      AND_WHEN("Reader unsubscribe from 2 topics")
      {
        reader.unsubscribe(0);
        reader.unsubscribe(1);
        THEN("Reader will have 0 subscriptions") { REQUIRE(reader.getSubscriptions().size() == 0); }
        AND_WHEN("Publisher publish on topic that was previously subscribed")
        {
          THEN("Reader will not receive any message") { REQUIRE(reader.messageReceived == 0); }
        }
      }
    }
    WHEN("Reader subscribe free function and member function on TIME topic")
    {
      const auto freeFunction = [&reader](const Newspaper &) { reader.freeFunctionCalled = true; };
      reader.subscribe("TIME", freeFunction);
      reader.subscribe("TIME", &Reader::readTime);
      AND_WHEN("Publisher will publish on TIME topic")
      {
        publisher->notify("TIME", Newspaper{ "Breaking news" });
        THEN("Both free function and member function will be called")
        {
          REQUIRE(reader.messageReceived == 1);
          REQUIRE(reader.timeRead == 1);
          REQUIRE(reader.freeFunctionCalled);
        }
      }
    }
  }
}