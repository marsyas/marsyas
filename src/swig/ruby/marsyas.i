%module marsyas

%include "../marsystem.i"
%include "../marsystemmanager.i"
%include "../marcontrolptr.i"

/* Automatic Type-Mapping For Marsystems */
%extend MarSystem {
                void setControl(std::string c,   mrs_bool v) { $self->setControl(c,v); }
                void setControl(std::string c,mrs_natural v) { $self->setControl(c,v); }
                void setControl(std::string c,   mrs_real v) { $self->setControl(c,v); }
                void setControl(std::string c, mrs_string v) { $self->setControl(c,v); }

                void updControl(std::string c,   mrs_bool v) { $self->updControl(c,v); }
                void updControl(std::string c,mrs_natural v) { $self->updControl(c,v); }
                void updControl(std::string c,   mrs_real v) { $self->updControl(c,v); }
                void updControl(std::string c, mrs_string v) { $self->updControl(c,v); }
};
