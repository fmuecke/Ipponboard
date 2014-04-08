#ifndef UTIL__DIALOGRESULT_H_
#define UTIL__DIALOGRESULT_H_

namespace FMU
{

template <typename T>
class DialogResult
{
public:
	explicit DialogResult(T const& data) : m_data(data)
	{}
	
    virtual T Result() const { return m_data; }

protected:
    T& DialogData() { return m_data; }

private:
	T m_data;
};

}

#endif  // UTIL__DIALOGRESULT_H_
