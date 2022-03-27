#include <functional>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>

/// @namespace dp
namespace dp {

/// Template for Subject class that manages subscriptions and notifications of events.
/// @tparam TTopic Hashable topic type that will be used as notification topic.
/// @tparam TArg Variable type that will be passed with each notification.
template<typename TTopic, typename TArg>
class TSubject {
 public:
  TSubject() = default;
  ~TSubject() = default;
  TSubject(const TSubject &a_other) = delete;
  TSubject(TSubject &&a_other) = default;

  TSubject &operator=(const TSubject &a_other) = delete;
  TSubject &operator=(TSubject &&a_other) = default;

  using Topic = TTopic;
  using VariableType = TArg;
  using Callback = std::function<void(const TArg &)>;
  using SubscriptionId = uint64_t;
  using Callbacks = std::unordered_map<SubscriptionId, Callback>;
  using DispatchTable = std::unordered_map<TTopic, Callbacks>;

  /// Subscribes listener under given topic.
  /// @param a_topic Topic to subscribe.
  /// @param a_cb Callback object that will be called whenever
  ///             'notify()' is called with given topic.
  /// @return uint64_t Id of a subscribed listener.
  uint64_t subscribe(const TTopic &a_topic, const Callback &a_cb)
  {
    m_dispatchTable[a_topic][m_counter] = a_cb;
    return m_counter++;
  }

  /// Unsubscribe listener entry under given id.
  /// @param a_id Id of a listener to unsubscribe.
  /// @retval true Listener was unsubscribed succesfully.
  /// @retval false Listener under given id was not found.
  bool unsubscribe(SubscriptionId a_id)
  {
    for (auto &[topic, callbacks] : m_dispatchTable) {
      const auto it = callbacks.find(a_id);
      if (it != callbacks.end()) {
        callbacks.erase(it);
        return true;
      }
    }
    return false;
  }

  /// Notifies all listeners that given value has changed.
  /// @param a_topic Topic to notify.
  /// @param a_arg Value that will be sent to all listeners.
  void notify(const TTopic &a_topic, const TArg &a_arg)
  {
    const auto it = m_dispatchTable.find(a_topic);
    if (it != m_dispatchTable.end()) {
      for (const auto &[id, cb] : it->second) {
        cb(a_arg);
      }
    }
  }

 private:
  DispatchTable m_dispatchTable{};
  uint64_t m_counter{};
};

/// Template for subscriber class that is used as a base class for all
/// subscribers/listeners. Template should be bound to specific type of
/// subject.
/// @tparam TSubject Subject class that will be used in TSubscriber context.
template<typename TSubject>
class TSubscriber {
 public:
  using Subscriptions = std::unordered_set<typename TSubject::SubscriptionId>;

  TSubscriber(const TSubscriber &a_other) = delete;
  TSubscriber(TSubscriber &&a_other) = default;

  TSubscriber &operator=(const TSubscriber &a_other) = delete;
  TSubscriber &operator=(TSubscriber &&a_other) = default;

  /// Creates TSubscriber object.
  /// @param a_subject pointer to subject pointer.
  explicit TSubscriber(const std::shared_ptr<TSubject> &a_subject)
    : m_subject{ a_subject }
  {
  }

  /// Destroys TSubscriber object.
  /// @note this destructor removes all subscriptions from underlying subject.
  ~TSubscriber() { unsubscribeAll(); }

  /// Subscribe to a given topic.
  /// @param a_topic Topic to subscribe.
  /// @param a_cb callback function to call on notify on given topic.
  void subscribe(const typename TSubject::Topic &a_topic, const typename TSubject::Callback &a_cb)
  {
    if (m_subject) {
      m_subscriptions.insert(m_subject->subscribe(a_topic, a_cb));
    }
  }

  /// Subscribe to a given topic.
  /// @tparam T Type that derives from TSubscriber and will be used to determine
  ///         the type of member function pointer.
  /// @param  a_topic Topic to subscribe.
  /// @param  a_cb Member function pointer that will be used as a callback
  ///              whenever notify on given topic is callled.
  template<typename T>
  void subscribe(const typename TSubject::Topic &a_topic, void (T::*a_cb)(const typename TSubject::VariableType &))
  {
    std::enable_if_t<std::is_base_of_v<TSubscriber, T>, T *> object = static_cast<T *>(this);
    if (m_subject) {
      m_subscriptions.insert(m_subject->subscribe(a_topic, [=](const typename TSubject::VariableType &a_arg) {
        if (object) {
          (object->*a_cb)(a_arg);
        }
      }));
    }
  }

  /// Unsubscribe from notifications under given id.
  /// @param a_id Id of a subscription.
  /// @retval true Subscription was removed.
  /// @retval false Subscription id was not found.
  bool unsubscribe(typename TSubject::SubscriptionId a_id)
  {
    if (m_subject) {
      const bool success = m_subject->unsubscribe(a_id);
      if (success) {
        m_subscriptions.erase(a_id);
      }
      return success;
    }
    return false;
  }

  /// Removes all subscriptions from underlying subject.
  void unsubscribeAll()
  {
    if (m_subject) {
      for (TSubject::SubscriptionId id : m_subscriptions) {
        m_subject->unsubscribe(id);
      }
    }
  }

  /// Gets all subscription id's.
  const Subscriptions &getSubscriptions() const { return m_subscriptions; }

 protected:
  Subscriptions m_subscriptions{};

 private:
  std::shared_ptr<TSubject> m_subject;
};

template<typename TTopic, typename TArg>
struct TObserver {
  struct Subject : public TSubject<TTopic, TArg> {
  };
  using Subscriber = TSubscriber<Subject>;
  using SubjectPtr = std::shared_ptr<Subject>;
};

template<typename TArg>
using BasicObserver = TObserver<std::string, TArg>;

template<typename... TArgs>
using VariantObserver = TObserver<std::string, std::variant<TArgs...>>;

} // namespace dp
