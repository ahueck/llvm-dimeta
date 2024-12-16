class Domain {
 public:
  Domain(int ranks, double other);

  int getRanks() const {
    return m_ranks;
  }

  double getOther() const {
    return m_other;
  }

 private:
  int m_ranks;
  double m_other;
};