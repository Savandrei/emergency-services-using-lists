// Savlovschi Andrei 311CB
// 120/120 on local checker
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int id;
	char type;
	int availability;
} UNIT;

typedef struct {
	int id;
	char priority[7];
	char *description;
	char status[11];
} INCIDENT;

typedef struct {
	INCIDENT *incident;
	UNIT *unit;
} INTERVENTION;

typedef struct unitsQueue {
	UNIT *pointer;
	struct unitsQueue *next;
} TCell_unit, *TQUnits;

typedef struct incident_list {
	INCIDENT info;
	struct incident_list *pre, *next;
} TCel_incident, *TLIncident;

// used for high, medium and low queues
typedef struct prioQueue {
	TLIncident pointer;
	struct prioQueue *next;
} Tprio_cell, *TQprio;

typedef struct intervention_list {
	INTERVENTION info;
	struct intervention_list *pre, *next;
} TCel_intervention, *TLIntervention;

typedef struct intervention_stack {
	INTERVENTION *pointer;
	struct intervention_stack *next;
} TScell, *TSInterv;

TLIncident Alocate_Incident(int ID, char priority[7], char* description) {

	TLIncident aux = (TLIncident) malloc(sizeof(TCel_incident));
	if (!aux) {
		return NULL;
	}
	aux->info.description = (char *) malloc(strlen(description) + 1);
	if (!aux->info.description) {
		return NULL;
	}

	aux->info.id = ID;
	strcpy(aux->info.status, "queued");
	strcpy(aux->info.priority, priority);
	strcpy(aux->info.description, description);
	aux->pre = NULL;
	aux->next = NULL;

	return aux;
}

TLIntervention Allocate_intervention(INCIDENT *incident, UNIT *unit) {

	TLIntervention cell = (TLIntervention)malloc(sizeof(TCel_intervention));
	if (!cell) {
		perror("Alloction failed\n");
		exit(77);
	}

	cell->next = NULL;
	cell->pre = NULL;
	cell->info.incident = incident;
	cell->info.unit = unit;

	return cell;
}

UNIT *Allocate_units(FILE *input, int unit_nr) {

	UNIT *units = (UNIT *) calloc(51, sizeof(UNIT));
	if (!units) {
		perror("Alloction failed\n");
		exit(77);
	}

	for (int i = 0; i < unit_nr; i++) {
		fscanf(input, "%d %c", &units[i].id, &units[i].type);
		units[i].availability = 1;
	}

	return units;
}

TQUnits Allocate_unit_cell(UNIT *pointer) {
	TQUnits cell = (TQUnits) malloc(sizeof(TCell_unit));
	if (!cell) {
		perror("Alloction failed\n");
		exit(77);
	}

	cell->pointer = pointer;
	cell->next = NULL;
	return cell;
}

TQprio Allocate_prio_cell(TLIncident pointer) {
	TQprio cell = (TQprio) malloc(sizeof(Tprio_cell));
	if (!cell) {
		perror("Alloction failed\n");
		exit(77);
	}

	cell->pointer = pointer;
	cell->next = NULL;
	return cell;
}

TSInterv Allocate_intervS_cell(INTERVENTION *pointer) {
	TSInterv cell = (TSInterv) malloc(sizeof(TScell));
	if (!cell) {
		perror("Alloction failed\n");
		exit(77);
	}

	cell->pointer = pointer;
	cell->next = NULL;
	return cell;
}

char *Allocate_string(int size) {
	char *command = malloc(size);
	if (!command) {
		perror("Alloction failed\n");
		exit(77);
	}
	return command;
}

TLIncident Init_Incident_List() {

	TLIncident aux = (TLIncident) malloc(sizeof(TCel_incident));
	if (!aux) {
		perror("Alloction failed\n");
		exit(77);
	}
	aux->info.description = (char *) malloc(9);
	if (!aux->info.description) {
		perror("Alloction failed\n");
		exit(77);
	}

	aux->info.id = 0;
	strcpy(aux->info.status, "solved");
	strcpy(aux->info.priority, "low");
	strcpy(aux->info.description, "sentinel");
	aux->pre = aux;
	aux->next = aux;

	return aux;
}

TQUnits Add_units_to_Q(TQUnits unitsQ, UNIT *pointer) {

	//empty queue case
	TQUnits end = unitsQ->next;
	if (end == NULL) {
		end = Allocate_unit_cell(pointer);
		end->next = end;
		return end;
	}

	//default case
	TQUnits new_cell, start = end->next;
	new_cell = Allocate_unit_cell(pointer);
	end->next = new_cell;
	new_cell->next = start;
	end = new_cell;
	return end;
}

// Adds pointers to incidents at the back of
// priority queues
TQprio Add_to_prioQ(TQprio prioQ, TLIncident pointer) {

	//empty queue case
	TQprio end = prioQ->next;
	if (end == NULL) {
		end = Allocate_prio_cell(pointer);
		end->next = end;
		return end;
	}

	//default case
	TQprio new_cell, start = end->next;
	new_cell = Allocate_prio_cell(pointer);
	end->next = new_cell;
	new_cell->next = start;
	end = end->next;
	return end;
}

// Used in UNDO_LAST_DISPATCH function
// Places the incidents that got postponed first
// in the priority queue
TQprio First_in_prioQ(TQprio prioQ, TLIncident pointer) {

	//empty queue case
	TQprio end = prioQ->next;
	if (end == NULL) {
		end = Allocate_prio_cell(pointer);
		end->next = end;
		return end;
	}

	//default case
	TQprio new_cell, start = end->next;
	new_cell = Allocate_prio_cell(pointer);
	end->next = new_cell;
	new_cell->next = start;
	return end;
}

TSInterv Add_to_interv_stack(TSInterv interv_stack, INTERVENTION *pointer) {

	//empty queue case
	TSInterv end = interv_stack->next;
	if (end == NULL) {
		end = Allocate_intervS_cell(pointer);
		end->next = end;
		return end;
	}

	//default case
	TSInterv new_cell, start = end->next;
	new_cell = Allocate_intervS_cell(pointer);
	end->next = new_cell;
	new_cell->next = start;
	end = end->next;
	return end;
}

INTERVENTION *Add_to_interventionL(TLIntervention intervention_s, INCIDENT *incident, UNIT *unit) {

	TLIntervention cell = Allocate_intervention(incident, unit);
	TLIntervention end = intervention_s->pre;
	cell->pre = end;
	cell->next = end->next;
	end->next->pre = cell;
	end->next = cell;

	return &(cell->info);
}

TQUnits Init_unitsQ(UNIT *units, int unit_nr) {

	//Queue pointer initialisation
	TQUnits unitsQ;
	unitsQ = (TQUnits)malloc(sizeof(TCell_unit));
	if (!unitsQ) {
		perror("Alloction failed\n");
		exit(77);
	}
	unitsQ->next = NULL;

	//Queue building
	for (int i = 0; i < unit_nr; i++)
		unitsQ->next = Add_units_to_Q(unitsQ, &units[i]);


	return unitsQ;
}

TQprio Init_prioQ() {

	TQprio prioQ;
	prioQ = (TQprio)malloc(sizeof(Tprio_cell));
	if (!prioQ) {
		perror("Alloction failed\n");
		exit(77);
	}
	prioQ->next = NULL;

	return prioQ;
}

TLIntervention Init_intervention_list() {

	TLIntervention s; //sentinel
	s = (TLIntervention)malloc(sizeof(TCel_intervention));
	if (!s) {
		perror("Alloction failed\n");
		exit(77);
	}
	s->info.incident = NULL;
	s->info.unit = NULL;
	s->next = s;
	s->pre = s;

	return s;
}

// This is not really a necessary function but
// I addded it to make a distinction for Initialisation
TSInterv Init_interv_stack() {

	TSInterv cell = Allocate_intervS_cell(NULL);

	return cell;
}

UNIT *Extr_unitQ(TQUnits unitsQ) {
	TQUnits end = unitsQ->next;
	TQUnits start = end->next;
	UNIT *info = start->pointer;
	if (start == end) {
		unitsQ->next = NULL;
	}

	end->next = start->next;
	free(start);

	return info;
}

TLIncident Extr_prioQ(TQprio prioQ) {

	TQprio end = prioQ->next;
	TQprio start = end->next;
	TLIncident info = start->pointer;
	if (start == end) {
		prioQ->next = NULL;
	}

	end->next = start->next;
	free(start);

	return info;
}

// Removes last intervention added to the stack
// UNDO_LAST_DISPATCH checks if the intervention was solved or not
INTERVENTION *Extr_interv_stack(TSInterv inter_stack) {
	// No elements
	TSInterv end = inter_stack->next;
	if (!end)
		return NULL;
	// 1 element
	INTERVENTION *info = end->pointer;
	if (end->next == end) {
		inter_stack->next = NULL;
		free(end);
		return info;
	}
	// 2 or more elements
	TSInterv last_bef_end = end->next;
	while (last_bef_end->next != end) {
		last_bef_end = last_bef_end->next;
	}
	last_bef_end->next = end->next;
	inter_stack->next = last_bef_end;
	free(end);
	return info;
}

// Removes the intervation from the list using
// the pointer on the intervention stack
void Remove_intervention(TSInterv interv_stack, TLIntervention pointer) {

	if (pointer == pointer->next) {
		interv_stack->next = NULL;
		free(pointer);
		return;
	}

	TLIntervention next_cell = pointer->next;
	TLIntervention prev_cell = pointer->pre;
	prev_cell->next = next_cell;
	next_cell->pre = prev_cell;
	free(pointer);
}

void Destroy_unitsQ(TQUnits *unitsQ_pointer) {
	TQUnits unitsQ = *unitsQ_pointer;
	TQUnits aux, end, start;
	end = unitsQ->next;

	if (end) {
		start = end->next;
		while (start != end) {
			aux = start;
			start = start->next;
			free(aux);
		}
		free(end);
	}

	free(unitsQ);
	unitsQ_pointer = NULL;
}

void Destroy_prioQ(TQprio *prioQ_pointer) {
	TQprio prioQ = *prioQ_pointer;
	TQprio aux, end, start;
	end = prioQ->next;
	if (end) {
		start = end->next;

		while (start != end) {
			aux = start;
			start = start->next;
			free(aux);
		}
		free(end);
	}

	free(prioQ);
	prioQ_pointer = NULL;
}

void Destroy_interv_stack(TSInterv *intervS_pointer) {
	TSInterv inter_stack = *intervS_pointer;
	TSInterv aux, end, start;
	end = inter_stack->next;
	if (end) {
		start = end->next;

		while (start != end) {
			aux = start;
			start = start->next;
			free(aux);
		}
		free(end);
	}

	free(inter_stack);
	intervS_pointer = NULL;
}

void Destroy_Incident_List(TLIncident *s) {
	TLIncident p = (*s)->next, aux;
	while (p != *s) {
		aux = p;
		p = p->next;
		free(aux->info.description);
		free(aux);
	}

	free((*s)->info.description);
	free(*s);
	*s = NULL;
}

void Destroy_Intervention_List(TLIntervention *s) {
	TLIntervention p = (*s)->next, aux;
	while (p != *s) {
		aux = p;
		p = p->next;
		free(aux);
	}

	free(*s);
	*s = NULL;
}

int ADD_INCIDENT(TLIncident s, TQprio highQ, TQprio medQ, TQprio lowQ, FILE *input) {

	int ID;
	char priority[7], brackets;
	char* description = Allocate_string(1000);

	// gets information from file about the incident
	fscanf(input, "%d %s %c", &ID, priority, &brackets);
	int i = -1;
	do {
		i++;
		fscanf(input, "%c", &description[i]);
	} while (description[i] != '"');
	description[i] = '\0';

	TLIncident aux = Alocate_Incident(ID, priority, description);

	// places the incident in their respective
	// priority queue
	if (strcmp(priority, "high") == 0)
		highQ->next = Add_to_prioQ(highQ, aux);
	else if (strcmp(priority, "medium") == 0)
		medQ->next = Add_to_prioQ(medQ, aux);
	else
		lowQ->next = Add_to_prioQ(lowQ, aux);

	// places the incident at the end of the incident list
	TLIncident ultim = s->pre;
	aux->pre = ultim;
	aux->next = ultim->next;
	ultim->next->pre = aux;
	ultim->next = aux;

	free(description);
	return 1;
}

int Number_of_units(TQUnits unitsQ) {
	if (unitsQ->next == NULL)
		return 0;
	TQUnits end = unitsQ->next;
	TQUnits start = end->next;
	int nr = 1;

	while (start != end) {
		nr++;
		start = start->next;
	}
	return nr;
}

void CHECK_UNITS_AVAILABILITY(TQUnits unitsQ, FILE *output) {

	int nr = Number_of_units(unitsQ);
	fprintf(output, "Number of available units: %d\n", nr);
}

void DISPATCH(TLIntervention interv_s, TSInterv interv_stack, TQUnits unitsQ, TQprio highQ, TQprio medQ, TQprio lowQ, FILE *output) {
	if (Number_of_units(unitsQ) == 0) {
		fprintf(output, "INVALID OPERATION! ERROR 404\n");
		return;
	}

	if (!highQ->next && !medQ->next && !lowQ->next) {
		fprintf(output, "INVALID OPERATION! ERROR 404\n");
		return;
	}

	// selects an incident in order of priority
	UNIT *unit_p = Extr_unitQ(unitsQ);
	TLIncident incident_p;
	if (highQ->next)
		incident_p = Extr_prioQ(highQ);
	else if (medQ->next)
		incident_p = Extr_prioQ(medQ);
	else
		incident_p = Extr_prioQ(lowQ);

	INTERVENTION *int_pointer = Add_to_interventionL(interv_s, &(incident_p->info), unit_p);

	interv_stack->next = Add_to_interv_stack(interv_stack, int_pointer);

	strcpy(incident_p->info.status, "intervened");
	unit_p->availability = 0;
}

void UNDO_LAST_DISPATCH(TSInterv interv_stack, TQUnits unitsQ, TQprio highQ, TQprio medQ, TQprio lowQ, FILE *output) {
	char priority[7];
	INTERVENTION *intervS_pointer;
	do {
		intervS_pointer = Extr_interv_stack(interv_stack);
	} while (intervS_pointer && strcmp(intervS_pointer->incident->status, "solved") == 0);

	if (!intervS_pointer)
		fprintf(output, "INVALID OPERATION! ERROR 404\n");

	else {
		strcpy(intervS_pointer->incident->status, "queued");

		strcpy(priority, intervS_pointer->incident->priority);
		if (strcmp(priority, "high") == 0)
			highQ->next = First_in_prioQ(highQ, (TLIncident)intervS_pointer->incident);
		else if (strcmp(priority, "medium") == 0)
			medQ->next = First_in_prioQ(medQ, (TLIncident)intervS_pointer->incident);
		else
			lowQ->next = First_in_prioQ(lowQ, (TLIncident)intervS_pointer->incident);

		intervS_pointer->unit->availability = 1;
		unitsQ->next = Add_units_to_Q(unitsQ, intervS_pointer->unit);

		Remove_intervention(interv_stack, (TLIntervention)intervS_pointer);
	}
}

void SOLVED_INCIDENT(TQUnits unitsQ, TLIntervention s_intervL, FILE *input, FILE *output) {

	int ID_incident;
	fscanf(input, "%d", &ID_incident);
	TLIntervention start = s_intervL->next;

	// looks for the intervention that includes the solved incident
	while (start->info.incident && ID_incident != start->info.incident->id) {
		start = start->next;
	}
	if (start->info.incident) {
		if (strcmp(start->info.incident->status, "intervened") == 0) {
			strcpy(start->info.incident->status, "solved");
			start->info.unit->availability = 1;
			unitsQ->next = Add_units_to_Q(unitsQ, start->info.unit);
		} else
			fprintf(output, "INVALID OPERATION! ERROR 404\n");
	} else
		fprintf(output, "INVALID OPERATION! ERROR 404\n");

}

void Show_Incident_list(TLIncident s) {
	TLIncident p = s->next;
	printf("Incident_list:\n");
	while (p != s) {
		printf("%d %s", p->info.id, p->info.priority);
		printf(" \"%s\"", p->info.description);
		printf(" %s\n", p->info.status);
		p = p->next;
	}
	printf("\n");
}

void Show_prioQ(TQprio prioQ) {
	TQprio end = prioQ->next;
	printf("prioQ_list:\n");
	if (!end)
		return;
	TQprio p = end->next;
	while (p != end) {
		printf("%d %s", p->pointer->info.id, p->pointer->info.priority);
		printf(" \"%s\"", p->pointer->info.description);
		printf(" %p", p->pointer);
		printf(" %s\n", p->pointer->info.status);
		p = p->next;
	}
	printf("%d %s", p->pointer->info.id, p->pointer->info.priority);
	printf(" \"%s\"", p->pointer->info.description);
	printf(" %p", p->pointer);
	printf(" %s\n", p->pointer->info.status);
	printf("\n");

}

void Show_interv_stack(TSInterv interv_stack) {
	TSInterv end = interv_stack->next;
	printf("intervention stack (from bottom to top of the stack):\n");
	if (!end)
		return;
	TSInterv p = end->next;
	while (p != end) {
		printf("INCIDENT ID: %d", p->pointer->incident->id);
		printf("  UNIT ID: %d\n", p->pointer->unit->id);
		p = p->next;
	}
	printf("INCIDENT ID: %d", p->pointer->incident->id);
	printf("  UNIT ID: %d\n", p->pointer->unit->id);

}

void Show_unitsQ(TQUnits unitsQ)
{
	TQUnits start = unitsQ->next->next;
	TQUnits end = unitsQ->next;
	TQUnits temp = start;
	int nr = 1;
	while (start != end) {
		nr++;
		start = start->next;
	}
	start = temp;
	printf("nr elem:%d\n", nr);

	printf("Coada: [");
	for(int i = 0; i < nr; i++) {
		printf("%p ", start->pointer);
		start = start->next;
	}

	printf("]\n");

}

int SHOW_UNIT(UNIT *units, FILE *input, FILE *output) {
	int ID, i = 0;
	fscanf(input, "%d", &ID);

	while (ID != units[i].id) {
		i++;
		if (i >= 51) {
			fprintf(output, "INVALID OPERATION! ERROR 404\n");
			return 0;
		}
	}

	fprintf(output, "Unit %d is type %c and is", ID, units[i].type);
	if (units[i].availability)
		fprintf(output, " available\n");
	else
		fprintf(output, " unavailable\n");

	return 1;
}

int SHOW_INCIDENT(TLIncident s, FILE *input, FILE *output) {

	int ID;
	fscanf(input, "%d", &ID);
	TLIncident aux = s->next;

	while (aux->info.id != ID) {
		if (aux == s) {
			fprintf(output, "INVALID OPERATION! ERROR 404\n");
			return 0;
		}
		aux = aux->next;
	}

	fprintf(output, "Incident %d has %s priority, ", aux->info.id, aux->info.priority);
	fprintf(output, "the following description: \"%s\" ", aux->info.description);
	fprintf(output, "and is %s\n", aux->info.status);
	return 1;
}

void SHOW_INTERVENTIONS(TLIntervention intervention_s, FILE *output) {
	TLIntervention start = intervention_s->next;
	if (start == intervention_s) {
		fprintf(output, "No intervention has been initiated\n");
		return;
	}
	while (start != intervention_s) {
		fprintf(output, "Incident %d was assigned", start->info.incident->id);
		fprintf(output, " to unit %d,", start->info.unit->id);
		fprintf(output, " and has the following status: \"%s\"\n", start->info.incident->status);
		start = start->next;
	}
}

int main(void) {
	// Initialization
	int unit_nr, cmd_nr;
	FILE *input = fopen("tema1.in", "rt");
	FILE *output = fopen("tema1.out", "wt");

	char *command = Allocate_string(100);
	fscanf(input, "%d", &unit_nr);
	UNIT *units = Allocate_units(input, unit_nr);
	TQUnits unitsQ = Init_unitsQ(units, unit_nr);
	TLIncident s_incident = Init_Incident_List();
	TLIntervention s_intervention = Init_intervention_list();
	TSInterv inter_stack = Init_interv_stack();
	TQprio queue_high = Init_prioQ();
	TQprio queue_medium = Init_prioQ();
	TQprio queue_low = Init_prioQ();

	// Commands
	fscanf(input, "%d", &cmd_nr);
	for (int i = 0; i < cmd_nr; i++) {
		fscanf(input, "%s", command);
		if (strcmp(command, "ADD_INCIDENT") == 0)
			ADD_INCIDENT(s_incident, queue_high, queue_medium, queue_low, input);

		else if (strcmp(command, "CHECK_UNITS_AVAILABILITY") == 0)
			CHECK_UNITS_AVAILABILITY(unitsQ, output);

		else if (strcmp(command, "DISPATCH") == 0)
			DISPATCH(s_intervention, inter_stack, unitsQ, queue_high, queue_medium, queue_low, output);

		else if (strcmp(command, "UNDO_LAST_DISPATCH") == 0)
			UNDO_LAST_DISPATCH(inter_stack, unitsQ, queue_high, queue_medium, queue_low, output);

		else if (strcmp(command, "SOLVED_INCIDENT") == 0)
			SOLVED_INCIDENT(unitsQ, s_intervention, input, output);

		else if (strcmp(command, "SHOW_UNIT") == 0)
			SHOW_UNIT(units, input, output);

		else if (strcmp(command, "SHOW_INCIDENT") == 0)
			SHOW_INCIDENT(s_incident, input, output);

		else if (strcmp(command, "SHOW_INTERVENTIONS") == 0)
			SHOW_INTERVENTIONS(s_intervention, output);

		// displays all queues and lists that dont have a specific command
		// done in console output for debugging and explaining code
		else if (strcmp(command, "list") == 0) {
			Show_Incident_list(s_incident);
			Show_prioQ(queue_high);
			Show_prioQ(queue_medium);
			Show_prioQ(queue_low);
			Show_interv_stack(inter_stack);
			printf("\n\n");
		}
	}

	// Deallocation
	Destroy_Incident_List(&s_incident);
	Destroy_Intervention_List(&s_intervention);
	Destroy_unitsQ(&unitsQ);
	Destroy_interv_stack(&inter_stack);
	Destroy_prioQ(&queue_high);
	Destroy_prioQ(&queue_low);
	Destroy_prioQ(&queue_medium);
	free(command);
	free(units);
	fclose(input);
	fclose(output);
	return 0;
}
