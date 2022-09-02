#Has to be before any imports, or the function will not be visible to the C API (don't sak me why)

from __future__ import print_function

import datetime
import os.path

from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from googleapiclient.discovery import build
from googleapiclient.errors import HttpError

# If modifying these scopes, delete the file token.json.
SCOPES = ['https://www.googleapis.com/auth/calendar.readonly']

class CalendarEvent:
    def __init__(self):
        isAllDay = False
        ending = False
        isLunar = False
        title = ""
        description = ""
        start = ""
        end = ""
        timezone = 0
        reminder = 0
        reminderMinutes = 0
        recursiveRule = 0
        typeID = ""

class CalendarAPI:
    def __init__(self):
        self.creds = None
        self.service = None
        self.events_result = None
        self.events = None

        self.configdir = ""

        self.load_0 = None
        self.requestJobs_0 = None
        self.requestJobs_1 = None

        self.load = self._load
        self.requestConnection = self._requestConnection
        self.requestJobs = self._requestJobs
        self.getJobsResult = self._getJobsResult
        self.getNextJob = self._getNextJob

        print("Google Calendar Service: CalendarAPI instance created.")

    def _load(self):
        print("Google Calendar Service: Token file: " + self.configdir + "/token.json");
        if (self.load_0):
            self.creds = Credentials.from_authorized_user_file(self.configdir + "/token.json", SCOPES)

        if (not self.creds or not self.creds.valid):
            if (self.creds and self.creds.expired and self.creds.refresh_token):
                self.creds.refresh(Request())
                with open(self.configdir + "/token.json", "w") as token:
                    token.write(creds.to_json())
                    print("Google Calendar Plugin: You are now connected.")
            else:
                print("Google Calendar Plugin: No Google Calendar data found, you're not connected, asking to connect.")
                self._requestConnection()
        else:
            print("Google Calendar Plugin: You are now connected.")

    def _requestConnection(self):
        print("Google Calendar Plugin: Requesting the user to connect.")
        flow = InstalledAppFlow.from_client_secrets_file("/home/nicolas/plugined/dde-calendar-5.9.1/calendar-client/plugins/google/credentials.json", SCOPES)
        self.creds = flow.run_local_server(port=0)
        with open(self.configdir + "/token.json", "w") as token:
            token.write(self.creds.to_json())
        self.service = build('calendar', 'v3', credentials=self.creds)

    def _requestJobs(self):
        print("Google Calendar Service: Getting the jobs.")
        print("Begin: ", self.requestJobs_0, " | ending: ", self.requestJobs_1, sep="")
        if (self.service):
        
            try:
                service = build('calendar', 'v3', credentials=creds)

                # Call the Calendar API
                now = datetime.datetime.utcnow().isoformat() + 'Z'  # 'Z' indicates UTC time
                print('Getting the upcoming 10 events')
                events_result = service.events().list(calendarId='primary', timeMin=now,
                                              maxResults=10, singleEvents=True,
                                              orderBy='startTime').execute()
                events = events_result.get('items', [])

                if not events:
                    print('No upcoming events found.')
                    return

                # Prints the start and name of the next 10 events
                for event in events:
                    start = event['start'].get('dateTime', event['start'].get('date'))
                    print(start, event['summary'])
                self.events = events

            except HttpError as error:
                print('An error occurred: %s' % error)

    def _getJobsResult(self):
        print("Google Calendar Service: Getting results.")
        if not self.events:
            print('No upcoming events found.')
        else:
            # Prints the start and name of the next 10 events
            for event in self.events:
                start = event['start'].get('dateTime', event['start'].get('date'))
                print(start, event['summary'])

    def _getNextJob(self):
        print("Google Calendar Service: Getting a job.")
        ce = CalendarEvent()
        if (not self.events):
            ce.ending = True
        else:
            ev = self.events.pop(0)
            print("Google Calendar Service: found event with the title:", ev["summary"])
            ce.title = ev["summary"]
            ce.description = ev["description"]
            ce.start = ev["start"].get("dateTime", ev["start"].get("date"))
            ce.end = ev["end"].get("dateTime", ev["end"].get("date"))
            ce.reminder = 0 #[TODO] Not supported yet.
            ce.reminderMinutes = 0 #[TODO] Not supported yet.
            ce.recursiveRule = 0 #[TODO] Not supported yet.
            ce.typeID = ev["eventType"]
            ce.isAllDay = (ce.start == ce.end) #Used to avoid checking all the formatting.
        return ce

