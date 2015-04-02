#define BOOST_TEST_MODULE CFindRequest
#include <boost/test/unit_test.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmnet/dimse.h>

#include "dcmtkpp/CFindRequest.h"
#include "dcmtkpp/ElementAccessor.h"
#include "dcmtkpp/Message.h"

#include "../MessageFixtureBase.h"

struct Fixture: public MessageFixtureBase<dcmtkpp::CFindRequest>
{
    DcmDataset command_set;
    DcmDataset query;

    Fixture()
    {
        dcmtkpp::ElementAccessor<EVR_US>::set(
            this->command_set, DCM_CommandField, DIMSE_C_FIND_RQ);
        dcmtkpp::ElementAccessor<EVR_US>::set(
            this->command_set, DCM_MessageID, 1234);
        dcmtkpp::ElementAccessor<EVR_UI>::set(
            this->command_set, DCM_AffectedSOPClassUID,
            UID_FINDPatientRootQueryRetrieveInformationModel);
        dcmtkpp::ElementAccessor<EVR_US>::set(
            this->command_set, DCM_Priority, DIMSE_PRIORITY_MEDIUM);

        dcmtkpp::ElementAccessor<EVR_PN>::set(
            this->query, DCM_PatientName, "Doe^John");
        dcmtkpp::ElementAccessor<EVR_LO>::set(
            this->query, DCM_StudyDescription, "Brain");
        dcmtkpp::ElementAccessor<EVR_CS>::set(
            this->query, DCM_QueryRetrieveLevel, "STUDY");
    }

    virtual void check(dcmtkpp::CFindRequest const & message)
    {
        BOOST_CHECK_EQUAL(message.get_command_field(), DIMSE_C_FIND_RQ);
        BOOST_CHECK_EQUAL(message.get_message_id(), 1234);
        BOOST_CHECK_EQUAL(
            message.get_affected_sop_class_uid(),
            UID_FINDPatientRootQueryRetrieveInformationModel);
        BOOST_CHECK_EQUAL(message.get_data_set(), &this->query);
    }
};

BOOST_FIXTURE_TEST_CASE(Constructor, Fixture)
{
    dcmtkpp::CFindRequest const message(
        1234, UID_FINDPatientRootQueryRetrieveInformationModel,
        DIMSE_PRIORITY_MEDIUM, &this->query);
    this->check(message);
}

BOOST_FIXTURE_TEST_CASE(MessageConstructor, Fixture)
{
    this->check_message_constructor(this->command_set, &this->query);
}

BOOST_FIXTURE_TEST_CASE(MessageConstructorWrongCommandField, Fixture)
{
    dcmtkpp::ElementAccessor<EVR_US>::set(
        this->command_set, DCM_CommandField, DIMSE_C_ECHO_RQ);
    this->check_message_constructor_throw(this->command_set, &this->query);
}

BOOST_FIXTURE_TEST_CASE(MessageConstructorMissingAffectSOPClass, Fixture)
{
    this->command_set.findAndDeleteElement(DCM_AffectedSOPClassUID);
    this->check_message_constructor_throw(this->command_set, &this->query);
}

BOOST_FIXTURE_TEST_CASE(MessageConstructorMissingPriority, Fixture)
{
    this->command_set.findAndDeleteElement(DCM_Priority);
    this->check_message_constructor_throw(this->command_set, &this->query);
}

BOOST_FIXTURE_TEST_CASE(MessageConstructorMissingQuery, Fixture)
{
    this->check_message_constructor_throw(this->command_set, NULL);
}

BOOST_FIXTURE_TEST_CASE(MessageConstructorEmptyQuery, Fixture)
{
    DcmDataset empty;
    this->check_message_constructor_throw(this->command_set, &empty);
}
