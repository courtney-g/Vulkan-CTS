/*-------------------------------------------------------------------------
 * drawElements Quality Program OpenGL ES 3.1 Module
 * -------------------------------------------------
 *
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *//*!
 * \file
 * \brief OpenGL ES 3.1 Test Package
 *//*--------------------------------------------------------------------*/

#include "tes31TestPackage.hpp"
#include "tes31InfoTests.hpp"
#include "es31fFunctionalTests.hpp"
#include "es31sStressTests.hpp"
#include "gluStateReset.hpp"
#include "gluRenderContext.hpp"
#include "tcuTestLog.hpp"

namespace deqp
{
namespace gles31
{

class TestCaseWrapper : public tcu::TestCaseExecutor
{
public:
									TestCaseWrapper		(TestPackage& package);
									~TestCaseWrapper	(void);

	void							init				(tcu::TestCase* testCase, const std::string& path);
	void							deinit				(tcu::TestCase* testCase);
	tcu::TestNode::IterateResult	iterate				(tcu::TestCase* testCase);

private:
	TestPackage&					m_testPackage;
};

TestCaseWrapper::TestCaseWrapper (TestPackage& package)
	: m_testPackage(package)
{
}

TestCaseWrapper::~TestCaseWrapper (void)
{
}

void TestCaseWrapper::init (tcu::TestCase* testCase, const std::string&)
{
	testCase->init();
}

void TestCaseWrapper::deinit (tcu::TestCase* testCase)
{
	testCase->deinit();

	DE_ASSERT(m_testPackage.getContext());
	glu::resetState(m_testPackage.getContext()->getRenderContext());
}

tcu::TestNode::IterateResult TestCaseWrapper::iterate (tcu::TestCase* testCase)
{
	tcu::TestContext&					testCtx	= m_testPackage.getContext()->getTestContext();
	const tcu::TestCase::IterateResult	result	= testCase->iterate();

	// Call implementation specific post-iterate routine (usually handles native events and swaps buffers)
	try
	{
		m_testPackage.getContext()->getRenderContext().postIterate();
		return result;
	}
	catch (const tcu::ResourceError& e)
	{
		testCtx.getLog() << e;
		testCtx.setTestResult(QP_TEST_RESULT_RESOURCE_ERROR, "Resource error in context post-iteration routine");
		testCtx.setTerminateAfter(true);
		return tcu::TestNode::STOP;
	}
	catch (const std::exception& e)
	{
		testCtx.getLog() << e;
		testCtx.setTestResult(QP_TEST_RESULT_FAIL, "Error in context post-iteration routine");
		return tcu::TestNode::STOP;
	}
}

TestPackage::TestPackage (tcu::TestContext& testCtx)
	: tcu::TestPackage	(testCtx, "dEQP-GLES31", "dEQP OpenGL ES 3.1 Tests")
	, m_archive			(testCtx.getRootArchive(), "gles31/")
	, m_context			(DE_NULL)
{
}

TestPackage::~TestPackage (void)
{
	// Destroy children first since destructors may access context.
	TestNode::deinit();
	delete m_context;
}

void TestPackage::init (void)
{
	try
	{
		// Create context
		m_context = new Context(m_testCtx);

		// Add main test groups
		addChild(new InfoTests						(*m_context));
		addChild(new Functional::FunctionalTests	(*m_context));
		addChild(new Stress::StressTests			(*m_context));
	}
	catch (...)
	{
		delete m_context;
		m_context = DE_NULL;

		throw;
	}
}

void TestPackage::deinit (void)
{
	TestNode::deinit();
	delete m_context;
	m_context = DE_NULL;
}

tcu::TestCaseExecutor* TestPackage::createExecutor (void) const
{
	return new TestCaseWrapper(const_cast<TestPackage&>(*this));
}

} // gles31
} // deqp
