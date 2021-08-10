#include <functional>
#include <map>
#include <variant>

namespace dp {

template<typename Topic, typename Arg>
class TObserver {
 public:
  enum NotifyStatus : int { eOk, eTopicNotFound };
  using Callback = std::function<void(const Arg &)>;
  using Id = uint64_t;

  uint64_t registerListener(const Topic &a_topic, const Callback &a_cb)
  {
    m_callbacks[a_topic][m_counter] = a_cb;
    return m_counter++;
  }

  NotifyStatus notifty(Topic &&a_topic, Arg &&a_arg)
  {
    const auto it = m_callbacks.find(a_topic);
    if (it != m_callbacks.end()) {
      for (const auto &[id, cb] : it->second) {
        cb(a_arg);
      }
      return eOk;
    }
    return eTopicNotFound;
  }

  bool unregisterListener(Id a_id)
  {
    for (auto &[topic, map] : m_callbacks) {
      const auto it = map.find(a_id);
      if (it != map.end()) {
        map.erase(it);
        return true;
      }
    }
    return false;
  }

 private:
  std::map<Topic, std::map<uint64_t, Callback>> m_callbacks{};
  uint64_t m_counter{};
};

template<typename Arg>
struct BasicObserver : public TObserver<std::string, Arg> {
};

template<typename... TArgs>
struct VariantObserver : public TObserver<std::string, std::variant<TArgs...>> {
};

} // namespace dp
