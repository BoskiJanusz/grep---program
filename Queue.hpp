
template <typename T>
class TQueue {
private:
    std::queue<T> m_queue;
  
    mutable std::mutex m_mutex;
  
    std::condition_variable m_cond;
  
public:
    void push(T item)
    {
        m_queue.push(item);
    }
  
    bool empty() const
    {
      std::lock_guard<std::mutex> lock (m_mutex);
      return m_queue.empty();
    }

    int size() const
    {
        return m_queue.size();
    }

    T pop()
    {
  
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock,
                    [this]() { return !m_queue.empty(); });
        T item = m_queue.front();
        m_queue.pop();
  
        return item;
    }
};