#ifndef UTIL__DIALOGRESULT_H_
#define UTIL__DIALOGRESULT_H_

namespace fm
{

template <typename T>
class DialogResult
{
public:
	explicit DialogResult(T const& data) : m_dialogData(data) {}
	virtual ~DialogResult() {}

	virtual T Result() const { return m_dialogData; }

protected:
	T m_dialogData;
};

}

#endif  // UTIL__DIALOGRESULT_H_
